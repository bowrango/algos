class Solution {
public:
    int countMajoritySubarrays(vector<int>& nums, int target) {
        // Convert nums using target=1 and non-target=-1 
        // Use prefix array pf[i] that represents the converted sum of the first i elements
        // The sum is therefore postive when target is the subarray majority
        std::vector<int> pf{0};
        int pfSum = 0;
        for(auto x : nums) {
            if(x==target) {pfSum+=1;}
            else {pfSum-=1;}
            pf.push_back(pfSum);
        }

        // Count frequency of each prefix sum
        std::unordered_map<int, int> f;
        f[0] = 1;
        pfSum = 0; //reset
        int count = 0;
        int n = pf.size();
        // For a subarray nums[l...r], the converted sum is pf[r+1] - pf[l].
        // positive when pf[r+1] > pf[l]
        for(int r = 1; r < n; ++r) {
            if(pf[r] > pf[r-1]) {pfSum += f[pf[r-1]];} // increment left count
            else {pfSum -= f[pf[r]];} // decrement right count
            count += pfSum;
            f[pf[r]] += 1; 
        }
        return count;
    }
};