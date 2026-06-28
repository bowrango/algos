class Solution {
public:
    int countMajoritySubarrays(vector<int>& nums, int target) {
    // Convert nums using target = +1 and non-target= -1
    // prefixSums[i] represents the converted sum of the previous i elements
    // Subarrays have target as the majority when the converted sum is positive
    std::vector<int> prefixSums{0};
    int thisPrefixSum = 0;
    for(auto x : nums) {
        if(x==target) {thisPrefixSum += 1;}
        else {thisPrefixSum -= 1;}
        prefixSums.push_back(thisPrefixSum);
    }
    // f[pfSum] = number of previous prefix sums equal to pfSum
    std::unordered_map<int, int> f;
    f[0] = 1;

    int numPrefixSumsLessThanCurrent = 0;
    int numMajority = 0;
    int n = prefixSums.size();
    // The subarray nums[l...r] has positive converted sum when 
    // prefixSums[r] > prefixSums[l]
    for(int r = 1; r < n; ++r) {
        if(prefixSums[r] > prefixSums[r-1]) {
            // Increment using previous
            numPrefixSumsLessThanCurrent += f[prefixSums[r-1]];
        } else {
            // Decrement using current
            numPrefixSumsLessThanCurrent -= f[prefixSums[r]];
        }
        numMajority += numPrefixSumsLessThanCurrent;
        // update
        f[prefixSums[r]] += 1;
    }
    return numMajority;
};
};