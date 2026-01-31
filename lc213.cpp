class Solution {
public:
    int linearrob(const vector<int>& nums) {
        int nm2 = 0; // dp[i-2]
        int nm1 = 0; // dp[i-1]
        for(int i=0; i<nums.size(); ++i) {
            int ni = std::max(nm2 + nums[i], nm1);
            nm2 = nm1;
            nm1 = ni;
        }
        return nm1;
    }

    int rob(vector<int>& nums) {
        if (nums.size()==0) {return 0;}
        if (nums.size()==1) {return nums[0];}
        // exclude last house
        int c1 = linearrob(std::vector<int>(nums.begin(), nums.end()-1));
        // include last house
        int c2 = linearrob(std::vector<int>(nums.begin()+1, nums.end()));
        return std::max(c1,c2);
    }
};