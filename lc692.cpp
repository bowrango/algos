class Solution {
public:
    // struct Compare {
    //     bool operator() (const auto& a, const auto& b) {
    //         if(a.second != b.second) {
    //             // compare frequency
    //             return a.second > b.second;
    //         }
    //         // compare strings
    //         return a.first < b.first;
    //     }
    // };
    vector<string> topKFrequent(vector<string>& words, int k) {
        unordered_map<string, int> mp;
        vector<string> result;
        // std::priority_queue<pair<string,int>, vector<pair<string,int>>, Compare> q;
        for(int i = 0; i < words.size(); i++) {
            mp[words[i]]++;
        }
        // [freq][strings]
        vector<vector<string>> freq(words.size()+1);
        for(auto it : mp) {
            freq[it.second].push_back(it.first);
        }
        // TODO improve runtime
        for(auto it : mp) {
            // sort each frequency bin
            sort(freq[it.second].begin(), freq[it.second].end());
        }
        for(int i = freq.size()-1; i >= 0; i--) {
            for(int j = 0; j < freq[i].size(); j++) {
                result.push_back(freq[i][j]);
                if(result.size()==k) {return result;}
            }
        }
        return result;
    }
};
