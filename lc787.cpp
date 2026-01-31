class Solution {
public:
    int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
        // build graph
        vector<vector<pair<int,int>>> adj(n);
        for(auto &f : flights) {
            adj[f[0]].push_back({f[1], f[2]});
        }
        int stops = 0;
        // dist[i] stores cheapest price to reach city i using <= current number of stops
        vector<int> dist(n, INT_MAX);
        dist[src] = 0;
        // (node, currentCost)
        queue<pair<int,int>> q;
        q.push({src, 0});
        // Run BFS for number of edges = 0 ... k+1 since k stops implies k+1 edges
        int u, v, cost;
        while(!q.empty() && stops<=k) {
            // number of edges to evaluate
            int sz = q.size();
            // use temporary dist to not override this layer
            vector<int> temp = dist;
            while(sz--) {
                auto [u, currentCost] = q.front();
                q.pop();
                if(currentCost > dist[u]) {continue;} // prune
                for(auto &nei : adj[u]) {
                   v = nei.first;
                   cost = nei.second;
                   if(currentCost + cost < temp[v]) {
                        temp[v] = currentCost + cost;
                        q.push({v, temp[v]});
                   }
                }
            }
            // update dist with new cheapest using <= stops+1 edges
            dist.swap(temp);
            stops++;
        }
        if(dist[dst]==INT_MAX) {return -1;}
        return dist[dst];

    }
    // int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
    //     Bellman-Ford
    //     int INF = INT_MAX;
    //     vector<int> dist(n, INF);
    //     dist[src] = 0;
    //     for(int step=0; step<=k; step++) {
    //         // create temp dist to update so previous iterations are not modified
    //         vector<int> temp = dist;
    //         for(auto f : flights) {
    //             int u = f[0];
    //             int v = f[1];
    //             int w = f[2];
    //             if(dist[u]==INF) {continue;}
    //             if(dist[u] + w < temp[v]) {
    //                 temp[v] = dist[u] + w;
    //             }
    //         }
    //         dist.swap(temp); // dist = temp without copy
    //     }
    //     if(dist[dst]==INF) {return -1;}
    //     return dist[dst];
    // }
};