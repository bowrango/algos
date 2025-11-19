class Solution {
public:
    int rob(vector<int>& nums) {
        
        int nm2 = 0; // dp[i-2]
        int nm1 = nums[0]; // dp[i-1]
        if (nums.size()==0) {return nm2;}
        if (nums.size()==1) {return nm1;}
        for(int i = 1; i < nums.size(); i++) {
            int rob = nm2 + nums[i];
            int skip = nm1;
            int ni = std::max(rob, skip);
            nm2 = nm1;
            nm1 = ni;
        }
        return nm1;
    }
};