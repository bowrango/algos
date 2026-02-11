#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

// Given an undirected graph G with V vertices where each vertex is an object with pointers to neighbors, determine the smallest N such that G can be considered an N-partite graph.

// In other words, find the smallest number of partitions such that no edge lies inside a partition. This is equivalent to finding the smallest N-Coloring of G (the chromatic number). Let D be the max node degree of G. Note that for every graph 1 <= N <= V. But a tighter bound is N <= D+1 since a color will always be free among D+1. We also know K <= N, for a clique size K.

// This can be solved approximately by a greedy coloring algorithm allowing D+1 colors. It returns a valid N >= C, where C is the chromatic number.

using namespace std;

struct Node {
    int id;
    vector<Node*> neighbors;
};
void addEdge(Node* a, Node* b) {
    a->neighbors.push_back(b);
    b->neighbors.push_back(a);
}

bool isBipartite(const vector<Node*>& G) {
    // Check bipartite using BFS 
    // Time O(V+E)
    // Space O(V)
    unordered_map<Node*, bool> color; // 0 or 1
    for (Node* v : G) {
        if (color.count(v)) {continue;} // already assigned

        queue<Node*> q;
        q.push(v);
        color[v] = 0;
        while (!q.empty()) {
            Node* u = q.front();
            q.pop();
            for (Node* v : u->neighbors) {
                if (v == u) {return false;} //self-loop

                if (!color.count(v)) {
                    // assign opposite color
                    color[v] = 1 - color[u];
                    q.push(v);
                } 
                else if (color[v] == color[u]) {return false;} // odd-cyle
            }
        }
    }
    return true;
}

int greedyColoring(const vector<Node*>& G) {
    // Approximate greedy coloring
    // Time O(V+E)
    // Space O(V)
    unordered_map<Node*, int> color;
    int maxColor = 0;
    for (Node* u : G) {
        // get colors of assigned neighbors
        unordered_set<int> used;
        for (Node* v : u->neighbors) {
            if (color.count(v)) {
                // neighboring color unavailable
                used.insert(color[v]);
            }
        }
        // assign next available color
        int c = 0;
        while (used.count(c)) {++c;}

        color[u] = c;
        maxColor = max(maxColor, c);
    }
    return maxColor + 1; //offset 0-based index
}

int chromaticNumber(const vector<Node*>& G) {
    // special cases
    if (G.empty()) {return 0;}
    if (G.size() == 1) {return 1;}
    if (isBipartite(G)) {return 2;}
    // general case
    return greedyColoring(G);
}

/*
 * Test 1: Path graph P5
 * N < V
 *
 * 1 — 2 — 3 — 4 — 5
 * N = 2
 */
void test_path_P5() {
    cout << "Test 1: Path P5 (N < V)\n";

    vector<Node> nodes(5);
    for (int i = 0; i < 5; ++i) nodes[i].id = i + 1;

    addEdge(&nodes[0], &nodes[1]);
    addEdge(&nodes[1], &nodes[2]);
    addEdge(&nodes[2], &nodes[3]);
    addEdge(&nodes[3], &nodes[4]);

    vector<Node*> G;
    for (auto& n : nodes) G.push_back(&n);

    cout << "Expected N = 2\n";
    cout << "Computed N = " << chromaticNumber(G) << "\n\n";
}

/*
 * Test 2: Odd cycle C5
 * N < V
 *
 * N = 3
 */
void test_cycle_C5() {
    cout << "Test 2: Odd cycle C5 (N < V)\n";

    vector<Node> nodes(5);
    for (int i = 0; i < 5; ++i) nodes[i].id = i + 1;

    for (int i = 0; i < 5; ++i) {
        addEdge(&nodes[i], &nodes[(i + 1) % 5]);
    }

    vector<Node*> G;
    for (auto& n : nodes) G.push_back(&n);

    cout << "Expected N = 3\n";
    cout << "Computed N = " << chromaticNumber(G) << "\n\n";
}

/*
 * Test 3: Complete graph K4
 * N = V
 *
 * N(K4) = 4
 */
void test_complete_K4() {
    cout << "Test 3: Complete graph K4 (N = V)\n";

    vector<Node> nodes(4);
    for (int i = 0; i < 4; ++i) nodes[i].id = i + 1;

    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            addEdge(&nodes[i], &nodes[j]);
        }
    }

    vector<Node*> G;
    for (auto& n : nodes) G.push_back(&n);

    cout << "Expected N = 4\n";
    cout << "Computed N = " << chromaticNumber(G) << "\n\n";
}

/*
 * Test 4: K4 minus one edge
 * N = d(G)
 *
 * d = 3, N = 3
 */
void test_K4_minus_edge() {
    cout << "Test 4: K4 minus one edge (N = d)\n";

    vector<Node> nodes(4);
    for (int i = 0; i < 4; ++i) nodes[i].id = i + 1;

    // Build K4
    addEdge(&nodes[0], &nodes[1]);
    addEdge(&nodes[0], &nodes[2]);
    addEdge(&nodes[0], &nodes[3]);
    addEdge(&nodes[1], &nodes[2]);
    addEdge(&nodes[1], &nodes[3]);
    // Missing edge: (2,3)

    vector<Node*> G;
    for (auto& n : nodes) G.push_back(&n);

    cout << "Expected N = 3 (d = 3)\n";
    cout << "Computed N = " << chromaticNumber(G) << "\n\n";
}

int main() {
    test_path_P5();
    test_cycle_C5();
    test_complete_K4();
    test_K4_minus_edge();
    return 0;
}