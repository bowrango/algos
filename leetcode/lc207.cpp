class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        // Build directed-graph from b to a so the graph is topologically sorted
        vector<vector<int>> g(numCourses);
        // Store in-degree for each course node
        vector<int> indeg(numCourses, 0);
        for(auto& e : prerequisites) {
            g[e[1]].push_back(e[0]);
            indeg[e[0]]++;
        }
        // Start from courses with no prerequisites
        queue<int> q;
        for(int i = 0; i < numCourses; ++i) {
            if(indeg[i]==0) {q.push(i);}
        }
        // Run BFS
        int numCompleted = 0;
        while(!q.empty()) {
            // Remove completed course
            int n = q.front();
            q.pop();
            numCompleted++;
            for(auto nei : g[n]) {
                indeg[nei]--;
                if(indeg[nei]==0) {
                    q.push(nei);
                }
            }
        }
        return numCompleted==numCourses;
    }
};