#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

// A point is supported iff at least one neighbor has angle >= 45deg
// ensure points are supported from left and right using two constaints:
// 1) since dx=1 and angle>=45deg, tan(theta) = abs(dy)/dx = 1 -> abs(dy) >= 1
// 2) new contour (output) is never below the input
std::vector<double> supportContour(const std::vector<double>& input) {
    int N = input.size();
    std::vector<double> left(N), right(N), output(N);

    // Pass left-to-right
    left[0] = input[0];
    for (int i = 1; i < N; ++i) {
        left[i] = std::max(input[i], left[i - 1] + 1.0);
    }

    // Pass right-to-left
    right[N - 1] = input[N - 1];
    for (int i = N - 2; i >= 0; --i) {
        right[i] = std::max(input[i], right[i + 1] + 1.0);
    }
    for (int i = 0; i < N; ++i) {
        output[i] = std::min(left[i], right[i]);
    }

    return output;
}

static bool near(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Pyramid already satisfies 45-deg slope, unchanged
    {
        auto out = supportContour({1, 2, 3, 2, 1});
        assert(near(out[0], 1) && near(out[1], 2) && near(out[2], 3));
        assert(near(out[3], 2) && near(out[4], 1));
    }

    // Test 2: Flat profile gets raised to form support slopes from edges
    {
        auto out = supportContour({5, 5, 5, 5});
        // Left:  5, 6, 7, 8   Right: 8, 7, 6, 5   Min: 5, 6, 6, 5
        assert(near(out[0], 5) && near(out[1], 6) && near(out[2], 6) && near(out[3], 5));
    }

    // Test 3: Single deep valley gets raised
    {
        auto out = supportContour({5, 0, 5});
        // Left: 5, 6, 7   Right: 7, 6, 5   Min: 5, 6, 5
        assert(near(out[0], 5) && near(out[1], 6) && near(out[2], 5));
    }

    // Test 4: Deep hole at the start
    {
        auto out = supportContour({0, 0, 5});
        // Left: 0, 1, 5   Right: 7, 6, 5   Min: 0, 1, 5
        assert(near(out[0], 0) && near(out[1], 1) && near(out[2], 5));
    }

    // Test 5: Single element
    {
        auto out = supportContour({42});
        assert(near(out[0], 42));
    }

    // Test 6: Wide valley between two peaks
    {
        auto out = supportContour({10, 0, 0, 0, 10});
        // Left:  10, 11, 12, 13, 14   Right: 14, 13, 12, 11, 10
        // Min:   10, 11, 12, 11, 10
        assert(near(out[0], 10) && near(out[1], 11) && near(out[2], 12));
        assert(near(out[3], 11) && near(out[4], 10));
    }

    return 0;
}
