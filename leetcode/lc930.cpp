class Solution {
public:
    int numSubarraysWithSum(vector<int>& nums, int goal) {
        // let prefixSum[i] = sum(nums[0:i]) but just store current iteration
        int prefixSum = 0;
        // sum of the subarray nums[l...r] equals goal when prefixSum[r]-goal == prefixSum[l]
        // f[prefixSum] = number of prefixSum occurences
        std::unordered_map<int, int> f;
        f[prefixSum] = 1;
        int count = 0;
        for(int r = 0; r < nums.size(); ++r) {
            prefixSum += nums[r];
            if(f.count(prefixSum - goal)) {count += f[prefixSum - goal];}
            // Update
            f[prefixSum]++;
        }
        return count;
    }
    };