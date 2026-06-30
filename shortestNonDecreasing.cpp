#include <vector>
#include <algorithm>
using namespace std;

// runtime O(n)
// space O(1)
int longestNonDecreasingSubarray(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return 1;

    int currLen = 1;
    int bestLen = 1;
    for (int i = 1; i < n; ++i) {
        if (nums[i] >= nums[i - 1]) {
            currLen++;
        } else {
            currLen = 1;
        }
        bestLen = max(bestLen, currLen);
    }
    return bestLen;
}

// runtime O(n)
// space O(n)
int longestNonDecreasingAfterOneChange(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return 1;

    vector<int> left(n, 1);
    vector<int> right(n, 1);

    // left[i] = longest non-decreasing run ending at i
    // nums = [2 3 4 1]
    // left = [1 2 3 1]
    for (int i = 1; i < n; ++i) {
        if (nums[i] >= nums[i - 1]) {
            left[i] = left[i - 1] + 1;
        }
    }

    // right[i] = longest non-decreasing run starting at i
    // nums = [5, 2, 3, 4]
    // right = [1 3 2 1]
    for (int i = n - 2; i >= 0; --i) {
        if (nums[i] <= nums[i + 1]) {
            right[i] = right[i + 1] + 1;
        }
    }
    
    // for each i:
    // pretend that we changed nums[i]
    // compute currLen using changed value
    int bestLen = 1;
    for (int i = 0; i < n; ++i) {
        // Change nums[i] and use it by itself
        int currLen = 1;

        // Connect to left side only
        if (i > 0) {
            currLen = max(currLen, left[i - 1] + 1);
        }

        // Connect to right side only
        if (i < n) {
            currLen = max(currLen, right[i + 1]);
        }

        // Connect both left and right sides
        if (i > 0 && i + 1 < n && nums[i - 1] <= nums[i + 1]) {
            currLen = max(currLen, left[i - 1] + 1 + right[i + 1]);
        }

        bestLen = max(bestLen, currLen);
    }
    return bestLen;
}