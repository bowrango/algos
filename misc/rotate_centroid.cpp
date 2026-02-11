#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

struct Point3D { double x, y, z; };

// Rotate N points around the Z-axis by `deg` degrees about their centroid.
// Returns the rotated points (translated so centroid is at origin).
std::vector<Point3D> rotateAroundCentroidZ(const std::vector<Point3D>& pts, double deg) {
    int N = pts.size();
    double cx = 0, cy = 0, cz = 0;
    for (auto& p : pts) { cx += p.x; cy += p.y; cz += p.z; }
    cx /= N; cy /= N; cz /= N;

    double theta = deg * M_PI / 180.0;
    double c = std::cos(theta), s = std::sin(theta);

    std::vector<Point3D> result(N);
    for (int i = 0; i < N; ++i) {
        double dx = pts[i].x - cx;
        double dy = pts[i].y - cy;
        double dz = pts[i].z - cz;
        // rotation matrix about Z is
        // [c -s  0
        //  s  c  0
        //  0  0  1]
        result[i] = { dx*c - dy*s, dx*s + dy*c, dz };
    }
    return result;
}

static bool near(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Single point at origin, rotation is identity
    {
        auto r = rotateAroundCentroidZ({{0, 0, 0}}, 90);
        assert(near(r[0].x, 0) && near(r[0].y, 0) && near(r[0].z, 0));
    }

    // Test 2: Two points symmetric about origin, 180-degree rotation swaps them
    {
        auto r = rotateAroundCentroidZ({{1, 0, 0}, {-1, 0, 0}}, 180);
        // centroid=(0,0,0), so (1,0)->(−1,0) and (−1,0)->(1,0)
        assert(near(r[0].x, -1) && near(r[0].y, 0));
        assert(near(r[1].x, 1)  && near(r[1].y, 0));
    }

    // Test 3: 90-degree rotation of (1,0) around origin gives (0,1)
    {
        auto r = rotateAroundCentroidZ({{1, 0, 5}}, 90);
        // single point, centroid is itself, so result is origin
        assert(near(r[0].x, 0) && near(r[0].y, 0) && near(r[0].z, 0));
    }

    // Test 4: Square vertices, 90-degree rotation permutes them
    {
        std::vector<Point3D> sq = {{1,1,0},{-1,1,0},{-1,-1,0},{1,-1,0}};
        auto r = rotateAroundCentroidZ(sq, 90);
        // centroid=(0,0,0); (1,1)->(-1,1), (-1,1)->(-1,-1), etc.
        assert(near(r[0].x, -1) && near(r[0].y, 1));
        assert(near(r[1].x, -1) && near(r[1].y, -1));
        assert(near(r[2].x, 1)  && near(r[2].y, -1));
        assert(near(r[3].x, 1)  && near(r[3].y, 1));
    }

    // Test 5: Z values are preserved
    {
        auto r = rotateAroundCentroidZ({{0, 0, 7}, {2, 0, 3}}, 45);
        assert(near(r[0].z, 7 - 5)); // cz=5, dz=2
        assert(near(r[1].z, 3 - 5)); // dz=-2
    }
    return 0;
}
