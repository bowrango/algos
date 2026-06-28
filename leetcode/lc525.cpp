class Solution {
public:
    int findMaxLength(vector<int>& nums) {
    // Convert 1/0 nums to +1/-1 which has an even count when sum = 0
    // pfSum = sum(nums[0:i])
    int pfSum = 0;
    // key pfSum; value last index with pfSum
    std::unordered_map<int, int> f;
    f[pfSum] = -1;
    int best = 0;
    for(int i = 0; i < nums.size(); ++i) {
        pfSum += nums[i]==1 ? 1 : -1;
        if(f.count(pfSum)) {best = std::max(best, i - f[pfSum]);}
        else {f[pfSum] = i;}
    }
    return best;
};
};