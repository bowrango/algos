class Solution {
public:
    vector<int> findMinHeightTrees(int n, vector<vector<int>>& edges) {
        // MHTs are in center of the graph where the max distance is minimum
        // Remove leaf nodes using BFS deque
        // Returns MHT roots which are the remaining leaves

        if(n==1) {return {0};}
        if(n==2) {return {0,1};}

        // symmetric graph
        vector<vector<int>> adj(n);
        vector<int> deg(n, 0);
        for(const auto &e : edges) {
            adj[e[0]].push_back(e[1]);
            adj[e[1]].push_back(e[0]);
            deg[e[0]]++;
            deg[e[1]]++;
        }
    
        deque<int> leaves; // ~25ms
        //queue<int> leaves; // ~31ms
        for(int i = 0; i < n; i++) {
            if(deg[i]==1) {
                // leaf node
                leaves.push_front(i);
                // leaves.push(i);
            }
        }

        int L, u;
        while(n > 2) {
            L = leaves.size();
            // remove all leaves
            n -= L;
            while(L--) {
                u = leaves.front();
                leaves.pop_front();
                // leaves.pop();
                for(int &v : adj[u]) {
                    // remove leaf
                    if(--deg[v]==1) {
                        leaves.push_back(v);
                        // leaves.push(v);
                    }
                }
            }
        }
        // Remaining leaves are the MHT roots
        vector<int> result;
        result.reserve(leaves.size());
        while(!leaves.empty()) {
            result.push_back(leaves.front());
            leaves.pop_front();
            // leaves.pop();
        }
        return result;
    }
};