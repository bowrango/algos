class Solution {
private:
pair<int, vector<int>> findFarthest(int start, const vector<vector<int>>& g) {
    int n = g.size();
    vector<int> dist(n, -1);
    dist[start] = 0;
    queue<int> q;
    q.push(start);
    int farthest = start;
    while(!q.empty()) {
        int node = q.front();
        q.pop();
        if(dist[node] > dist[farthest]) {
            farthest = node;
        }
        for(int nei : g[node]) {
            if(dist[nei]==-1) {
                dist[nei] = dist[node]+1;
                q.push(nei);
            }
        }
    }
    return {farthest, dist};
}
public:
    string findSpecialNodes(int n, vector<vector<int>>& edges) {
        if(n==1) return "1";
        if(n==2) return "11";
        // Notes:
        // Node endpoints have degree 1 but not every endpoint is strictly special
        // The tree has no cycles so use 2-pass BFS to find diameter paths
        string result(n, '0');
        vector<vector<int>> g(n);
        for(const auto& e : edges) {
            g[e[0]].push_back(e[1]);
            g[e[1]].push_back(e[0]);
        }

        // Start BFS from arbitrary node
        // Farthest node from the start will be an endpoint A
        auto [A, distStart] = findFarthest(0, g);
        // Farthest node from A is also an endpoint B
        auto [B, distA] = findFarthest(A, g);
        auto [C, distB] = findFarthest(B, g);
        int diameter = distA[B];
        // distA[i] = distance from node i to A
        // distB[i] = distance from node i to B
        for(int i = 0; i < n; ++i) {
            if(max(distA[i], distB[i])==diameter) {
                result[i] = '1';
            }
        }
        return result;
    }
};