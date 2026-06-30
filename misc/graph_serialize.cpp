
// Encode graph to vector and decode vector to graph

// [N, 
//  for each node:
//  id,
//  data, 
//  deg,
//  edge ids]; 

// encode/decode
#include <vector>
#include <unordered_map>
#include <memory>
// tests
#include <stdexcept>
#include <iostream>
#include <cassert>

struct Node {
    int data;
    std::vector<Node*> edges;
};

std::vector<int> encode(const std::vector<Node*>& G) {
    // idMap[n] = unique node identier
    // [N, [nId, data, deg, [eIds]], [nId, data, deg, [eIds]], ...]
    std::unordered_map<Node*, int> idMap;
    int N = G.size();
    std::vector<int> data;
    data.push_back(N);
    for(auto n : G) {
        if(idMap.count(n)) {
            int id = idMap[n];
            data.push_back(id);
        } else {
            int id = idMap.size();
            idMap[n] = id;
            data.push_back(id);
        }
        data.push_back(n->data);
        int deg = n->edges.size();
        data.push_back(deg);
        for(auto e : n->edges) {
            if(idMap.count(e)) {
                int id = idMap[e];
                data.push_back(id);
            } else {
                int id = idMap.size();
                data.push_back(id);
                idMap[e] = id;
            }
        }
    }
    return data;
}

std::vector<Node*> decode(const std::vector<int>& data) {
    // nodeMap[nId] = unique pointer to nId node
    std::unordered_map<int, Node*> nodeMap;
    // {nId, {eIds}}
    std::vector<std::tuple<int, std::vector<int>>> edges;
    std::vector<Node*> graph;

    // Build all nodes
    int idx = 0;
    int N = data[idx++];
    for(int i = 0; i < N; ++i) {
        int nId = data[idx++];
        Node* n = new Node();
        n->data = data[idx++];
        nodeMap[nId] = n;

        graph.push_back(n);

        int deg = data[idx++];
        std::vector<int> edgeIds;
        for(int j = 0; j < deg; ++j) {
            edgeIds.push_back(data[idx++]);
        }
        edges.push_back({nId, edgeIds});
    }

    // Connect all nodes
    for(auto& [nId, edgeIds] : edges) {
        Node* n = nodeMap[nId];
        for(int eId : edgeIds) {
            n->edges.push_back(nodeMap[eId]);
        }
    }

    return graph;
}

int main() {

    Node* n0 = new Node();
    Node* n1 = new Node();
    Node* n2 = new Node();
    n0->data = 0;
    n1->data = 1;
    n2->data = 2;
    n0->edges = {n1, n2};

    std::vector<Node*> inG = {n0, n1, n2};

    std::vector<int> ec0 = encode(inG);
    for(auto x : ec0) {
        std::cout << x << std::endl;
    }

    std::vector<Node*> outG = decode(ec0);

    std::cout << "=============" << std::endl;

    std::vector<int> ec1 = encode(outG);
    for(auto x : ec1) {
        std::cout << x << std::endl;
    }

    return 0;
}
