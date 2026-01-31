class Solution {
public:
    double maxProbability(int n, vector<vector<int>>& edges, vector<double>& probs, int start, int end) {
        // Use Dijkstra with -logprob weights and minheap queue
        // Time: Binary heap uses O(logn), and with E edges we have complexity O(Elogn)
        // Space: Store weight vector length n and g with all edges so complexity is O(n + E)

        // Build graph
        vector<vector<pair<int, double>>> g(n);
        for (size_t k=0; k < edges.size(); k++) {
            int u = edges[k][0];
            int v = edges[k][1];
            double w = -log(probs[k]);
            g[u].push_back({v,w});
            g[v].push_back({u,w});
        }

        // Keep track of weights for each node
        double INF = numeric_limits<double>::infinity();
        vector<double> weight(n, INF);
        weight[start] = 0.0; // log(1) = 0

        // Build queue. The state with lowest weight is at the top
        using s = pair<double, int>;
        priority_queue<s, vector<s>, greater<s>> q;
        q.push({0.0, start});

        // Traverse graph
        while (!q.empty()) {
            auto [wu, u] = q.top();
            q.pop();
            if (u==end) {return exp(-wu);};
            if (wu > weight[u]) {continue;}; // Dead end
            for (auto [v, wv] : g[u]) { // Go through neighbors
                double nw = wu + wv;
                if (nw < weight[v]) {
                    weight[v] = nw;
                    q.push({nw, v});
                }
            }
        }
        return 0.0;
    }
};