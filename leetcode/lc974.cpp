class Solution {
public:
    int subarraysDivByK(vector<int>& nums, int k) {
        // pfSum = sum(nums[0:r+1])
        int pfSum = 0;
        // key is previous remainders modulo k, value is counts
        std::unordered_map<int,int> f;
        f[0] = 1; // pfSum % k = 0
        int n = nums.size();
        int count = 0;
        for(int r = 0; r < n; ++r) {
            pfSum += nums[r];
            int rem = ((pfSum % k) + k) % k; // 0 <= rem <= k-1
            // Count how many previous pfSums had this remainder
            count += f[rem];
            // Update
            f[rem] += 1;
        }
        return count;
};
};