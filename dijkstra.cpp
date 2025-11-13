
#include <queue> 
#include <iostream>

// g++ dijkstra.cpp -o dijkstra -std=c++17

std::vector<int> Dijkstra(int n, std::vector<std::vector<int>>& edges, std::vector<double> w, int src, int trgt) {
    // Returns shortest-path using Dijkstra algorithm. Uses minheap for runtime complexity O((E+V)logV)

    // build graph 
    std::vector<std::vector<std::pair<double, int>>> g(n);
    for (int k = 0; k < edges.size(); ++k) {
        int u = edges[k][0];
        int v = edges[k][1];
        g[u].push_back({w[k], v});
        // make symmetric
        g[v].push_back({w[k], u});
    }

    // track distance
    double INF = std::numeric_limits<double>::infinity();
    std::vector<double> dist(n, INF);
    // track previous node on shortest path
    std::vector<int> parent(n, -1);

    // minheap keeps lowest weight at the top
    using s = std::pair<double, int>;
    std::priority_queue<s, std::vector<s>, std::greater<s>> q;
    q.push({0.0, src});
    dist[src] = 0.0;

    while (!q.empty()) {
        auto [du, u] = q.top();
        q.pop();
        if (u == trgt) {break;}
        if (du > dist[u]) {continue;}
        for (auto [dv, v] : g[u]) {
            double d = dv + du;
            if (d < dist[v]) {
                q.push({d, v});
                parent[v] = u;
                dist[v] = d;
            }
        }
    }

    // build path
    std::vector<int> path;
    for (int cur = trgt; cur!=-1; cur = parent[cur]) {
        path.push_back(cur);
    }
    reverse(path.begin(), path.end());
    return path;
}

int main() {

    int n = 6;
    std::vector<std::vector<int>> e = {{0,1},{3,0},{0,2},{1,2},{2,4},{3,4},{0,5}};
    std::vector<double> w = {1.0, 1.0, 3.0, 1.0, 1.0, 0.0};
    std::vector<int> res = Dijkstra(n, e, w, 0, 4);
    for (auto idx : res) {
        std::cout << idx << std::endl;
    }

    return 0;
}