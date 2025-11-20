class Solution {
public:
    bool wordBreak(string s, vector<string>& wordDict) {
            // m := length of wordDict
            // k := average word length
            // Time O(n*k^2 + mk)
            // Space O(n + mk)
            int n = s.size();
            int L = getMaxLength(wordDict);
            unordered_set<string> wordSet(begin(wordDict), end(wordDict));
            // tf[i] is whether the substring s[0 ... i-1] can be segmented
            vector<bool> tf(n+1, false);
            tf[0] = true; // empty string
            for(int i = 1; i<=n; ++i) {
                for(int j=i-1; j>=0; --j) {
                    if (i-j <= L) {
                        if(tf[j] && wordSet.count(s.substr(j, i-j))) {
                            tf[i] = true;
                            break;
                        }
                    }
                }
            }
            return tf[n];
        }
private:
    int getMaxLength(vector<string>& wordDict) {
        auto it = max_element(begin(wordDict), end(wordDict), [](const string& a, const string& b) {
            return a.size() < b.size();
        });
        return it->size();
    }
};