class Solution {
public:
    string minWindow(string s, string t) {
        if(s.empty() || t.empty() || s.size() < t.size()) {
            return "";
        }
        size_t m = s.size();
        // frequency map (unsigned char index)
        // a-z (65-90) A-Z (97-122)
        vector<int> need(128, 0);
        for(char c : t) {
            need[c]++;
        }
        size_t l = 0;
        size_t start = l;
        size_t missing = t.size();
        size_t L = INT_MAX; // minimum window size
        for(size_t r = 0; r < m; r++) {
            // extend from right
            if(need[s[r]] > 0) {
                missing--;
            }
            need[s[r]]--;

            while(missing==0) {
                if(r-l+1 < L) {
                    // update
                    L = r-l+1;
                    start = l;
                }
                need[s[l]]++;
                if(need[s[l]] > 0) {
                    missing++;
                }
                // extend from left
                l++;
            }
        }
        if(L==INT_MAX) {
            return "";
        } else {
            return s.substr(start, L);
        }
    }
};