#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <cassert>

// lazyStack supports O(1) operations: "push x", "pop", "inc i v"

static const long long EMPTY = LLONG_MIN;

std::vector<long long> lazyStack(const std::vector<std::string>& operations) {
    // Maximum possible stack depth is <= number of operations (all could be pushes).
    // Use long long because increments can accumulate to ~2e14 (2e5 * 1e9),
    // which can overflow 32-bit int.
    int sz = operations.size();
    std::vector<long long> val(sz), inc(sz);
    std::vector<long long> results;

    // n is only a count/index: 0 <= n <= operations_size <= 2e5, so int is safe.
    int n = 0;

    for (int k = 0; k < sz; ++k) {
        const std::string& op = operations[k];
        if (op[1] == 'u') {
            // push
            long long v = std::stoll(op.substr(5));
            val[n] = v;   // store raw pushed value at new top index
            inc[n] = 0;   // no pending increment at this depth yet
            n++;          // stack grows
        } else if (op[1] == 'o') {
            // pop
            n--;  // new top index is n-1
            if (n > 0) {
                // Carry increments from removed top n to the new top n-1
                // Bottom elements are exactly "bottom n elements", so we add it to inc[n-1].
                inc[n - 1] += inc[n];
            }
        } else if (op[0] == 'i') {
            // inc i v
            // Add v to the bottom i elements.
            // Record this increment at inc[i-1], not on each element.
            size_t pos1 = 4;
            size_t pos2;
            long long i = std::stoll(op.substr(pos1), &pos2);
            long long v = std::stoll(op.substr(pos1 + pos2));

            // Convert bottom i elements to an index in [0, n-1]
            int idx = static_cast<int>(i) - 1;

            // Guard in case i > n
            if (idx >= 0 && idx < n) {
                // Defer adding v until those elements bubble up via pops
                inc[idx] += v;
            }
        }

        // Current top is val[n-1] + inc[n-1] = (raw top value) + (total pending increment at top)
        if (n == 0) {
            results.push_back(EMPTY);
        } else {
            results.push_back(val[n - 1] + inc[n - 1]);
        }
    }

    return results;
}

int main() {
    // Test 1: Basic push and pop
    {
        auto r = lazyStack({"push 4", "push 5", "pop", "pop"});
        assert(r[0] == 4);         // push 4 -> top=4
        assert(r[1] == 5);         // push 5 -> top=5
        assert(r[2] == 4);         // pop    -> top=4
        assert(r[3] == EMPTY); // pop    -> EMPTY
    }

    // Test 2: Push, inc, pop sequence
    {
        auto r = lazyStack({"push 1", "push 2", "inc 2 10", "pop", "pop"});
        assert(r[0] == 1);         // push 1
        assert(r[1] == 2);         // push 2
        assert(r[2] == 12);        // inc bottom 2 by 10 -> top = 2+10 = 12
        assert(r[3] == 11);        // pop -> top = 1+10 = 11
        assert(r[4] == EMPTY);
    }

    // Test 3: Multiple increments accumulate
    {
        auto r = lazyStack({"push 0", "inc 1 5", "inc 1 3", "pop"});
        assert(r[0] == 0);
        assert(r[1] == 5);         // 0 + 5
        assert(r[2] == 8);         // 0 + 5 + 3
        assert(r[3] == EMPTY);
    }

    // Test 4: Inc only affects bottom i elements
    {
        auto r = lazyStack({"push 1", "push 2", "push 3", "inc 1 100", "pop", "pop", "pop"});
        assert(r[0] == 1);
        assert(r[1] == 2);
        assert(r[2] == 3);
        assert(r[3] == 3);         // inc bottom 1 by 100, top is still 3
        assert(r[4] == 2);         // pop -> top=2 (not incremented)
        assert(r[5] == 101);       // pop -> top=1+100
        assert(r[6] == EMPTY);
    }

    // Test 5: Single push then pop
    {
        auto r = lazyStack({"push 10", "pop"});
        assert(r[0] == 10);
        assert(r[1] == EMPTY);
    }

    return 0;
}
