class Solution {
public:
    long long countSubarrays(vector<int>& nums, long long k) {
    // Use sliding window (l, r) that expands right-wards and shrinks left-wards
    // Track score = sum(nums[l:r])*(r + 1 - l) using running sum
    long long sum = 0;
    long long count = 0;
    // Maintain valid l endpoint for each r
    int l = 0;
    for(int r = 0; r < nums.size(); ++r) {
        sum += nums[r];
        // Shrink until valid
        while(sum*(r + 1 - l) >= k) {
            sum -= nums[l];
            l++;
        }
        // Window is the largest valid subarray ending at r, which
        // includes shorter valid subarrays since nums is positive.
        // Increment the count by the length since the valid subarrays
        // are [l:r],[l+1:r],...,[r:r].
        count += r + 1 -l;
    }
    return count;
};
};