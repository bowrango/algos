#include <vector>

// Given an array of integers, define the score of a subarray as the sum of its first and last elements, minus the sum of all its
// intermediate elements.
// Find the maximum possible score of any subarray within the given array.

// score(i,j) = nums[i]+nums[j] - sum_{k=i+1}{j-1} a[k]

// Define pfSum[i] = sum(nums[i])

// score(i,j) = nums[i] + nums[j] - (pfSum[j] - pfSum[i+1])
// score(i,j) = (pfSum[i+1] + nums[i]) - (pfSum[j] - nums[j])
// score(i,j) = (pfSum[i] + 2*nums[i]) + (nums[j] - pfSum[j])
// Keep maximum value of left side as we can from left to right

int score(std::vector<int>& nums) {
    size_t n = a.size();
    if(n < 2) {
        throw runtime_error("need n >= 2");
    }

    int bestLeft = 2*nums[0];
    int pfSum = nums[0]; // pfSum[r]
    int score = std::numerical_limits<int>::min();
    for(int r = 1; r < nums().size(); ++r) {
        score = max(score, bestLeft + nums[r]-pfSum)
        bestLeft = max(bestLeft, pfSum+2*nums[r]);
        pfSum += nums[r];
    }
    return score;
}

int main() {
    return 0;
}