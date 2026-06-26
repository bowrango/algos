class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        // Use a prefix sum pfSum[i] = sum(nums[0:i])
        int pfSum = 0;
        // Store counts of previous pfSums
        std::unordered_map<int, int> f;
        f[0] = 1;
        int count = 0;
        // The subarray nums[i...j] has the sum pfSum[j+1] - pfSum[i]
        for(auto x : nums) { // Treat x = j+1
            pfSum += x;
            // Count how many times pfSum[j+1] - k = pfSum[i] 
            count += f[pfSum - k];
            // Update
            f[pfSum]+=1;
        }
        return count;
    }
};