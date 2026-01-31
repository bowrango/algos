class Solution {
public:
    bool canFinish(int n, vector<vector<int>>& prereq) {
        // Determine if the directed graph has a cycle. A cycle implies it's impossible.
        vector<vector<int>> g(n); // sparse
        queue<int> q;
        vector<int> indeg(n, 0);
        // Build dependency graph
        for(auto& c : prereq) { // [a b]
            // Store courses that depend on course b
            g[c[1]].push_back(c[0]);
            indeg[c[0]]++;
        }
        for(int i = 0; i < n; i++) {
            // Store courses with no prereqs
            if(indeg[i]==0) {q.push(i);}
        }
        
        // Run BFS
        int count = 0; // Track number of "taken" courses
        while(!q.empty()) {
            int c = q.front();
            q.pop();
            count++;
            for(auto& nei : g[c]) {
                indeg[nei]--;
                if(indeg[nei]==0) {
                    // 
                    q.push(nei);
                }
            }
        }
        return count==n;
     }
};