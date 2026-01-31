
// Basic graph encoder/decoder for
// vector<Node*> <-> vector<int>
// using the format
// [N,
//  for each node:
//    data,
//    deg,
//    e0, e1, ..., e(deg-1)
// ]

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
    // Assign node IDs
    std::unordered_map<const Node*, int> id;
    const int N = G.size();
    id.reserve(N);
    for(int i = 0; i < N; ++i) {
        if(!G[i]) {throw std::runtime_error("invalid node");}
        if(id.count(G[i])) {throw std::runtime_error("duplicate node");}
        id[G[i]] = i;
    }

    std::vector<int> out;
    out.reserve(1 + 3*N); // estimate
    out.push_back(N);
    for(int i = 0; i < N; ++i) {
        Node* n = G[i];
        out.push_back(n->data);
        out.push_back(static_cast<int>(n->edges.size()));
        for(Node* m : n->edges) {
            auto it = id.find(m);
            if(it == id.end()) {throw std::runtime_error("invalid node edge");}
            out.push_back(it->second);
        }
    }
    return out;
}

std::vector<std::unique_ptr<Node>> decode(const std::vector<int>& v) {
    if(v.empty()) {throw std::runtime_error("empty");}
    int N = v[0];
    if(N < 0) {throw std::runtime_error("invalid node count");}

    std::vector<std::unique_ptr<Node>> nodes;
    nodes.reserve(N);

    // Allocate nodes and read data
    int idx = 1;
    for(int i = 0; i < N; ++i) {
        if(idx >= static_cast<int>(v.size())) {throw std::runtime_error("truncated data");}
        auto n = std::make_unique<Node>();
        n->data = v[idx++];
        nodes.push_back(std::move(n));

        if(idx >= static_cast<int>(v.size())) {throw std::runtime_error("truncated degree");}
        int deg = v[idx++];
        if(deg < 0) {throw std::runtime_error("negative degree");}
        if(idx + deg > static_cast<int>(v.size())) {throw std::runtime_error("truncated edges");}
        idx += deg;
    }

    // Connect edges
    idx = 1;
    for(int i = 0; i < N; ++i) {
        idx++; // skip data
        int deg = v[idx++];
        nodes[i]->edges.reserve(deg);
        for(int k = 0; k < deg; ++k) {
            int e = v[idx++];
            if(e < 0 || e >= N) {throw std::runtime_error("invalid edge");}
            nodes[i]->edges.push_back(nodes[e].get());
        }
    }
    if(idx != static_cast<int>(v.size())) { throw std::runtime_error("trailing data");}
    return nodes;
}

void print(const std::vector<int>& v) {
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";
}

void test_single_node() {
    Node n{42, {}};

    std::vector<Node*> G = {&n};
    auto v = encode(G); // [1 42 0]

    assert(v.size() == 3);
    assert(v[0] == 1);
}

void test_two_isolated_nodes() {
    Node n1{1, {}};
    Node n2{2, {}};

    std::vector<Node*> G = {&n1, &n2};
    auto v = encode(G); // [2 1 0 2 0]

    assert(v.size() == 5);
    assert(v[0] == 2);
}

void test_one_edge() {
    Node n1{10, {}};
    Node n2{20, {}};
    n1.edges.push_back(&n2);

    std::vector<Node*> G = {&n1, &n2};
    auto v = encode(G); // [2 10 1 1 20 0]

    assert(v.size() == 6);
    assert(v[0] == 2);
}

void test_cycle() {
    Node n1{1, {}};
    Node n2{2, {}};
    Node n3{3, {}};

    n1.edges = {&n2};
    n2.edges = {&n3};
    n3.edges = {&n1};

    std::vector<Node*> G = {&n1, &n2, &n3};
    auto v = encode(G); // [3, 1,1,1, 2,1,2, 3,1,0]

    assert(v.size() == 10);
    assert(v[0] == 3);
}

void test_empty_graph() {
    std::vector<Node*> G = {};
    auto v = encode(G); // [0]

    assert(v.size() == 1);
    assert(v[0] == 0);

    auto decoded = decode(v);
    assert(decoded.empty());
}

void test_self_loop() {
    Node n{99, {}};
    n.edges.push_back(&n);

    std::vector<Node*> G = {&n};
    auto v = encode(G); // [1, 99, 1, 0]

    assert(v.size() == 4);
    assert(v[0] == 1);
    assert(v[1] == 99);
    assert(v[2] == 1);
    assert(v[3] == 0);

    auto decoded = decode(v);
    assert(decoded.size() == 1);
    assert(decoded[0]->data == 99);
    assert(decoded[0]->edges.size() == 1);
    assert(decoded[0]->edges[0] == decoded[0].get());
}

void test_round_trip() {
    Node n1{10, {}};
    Node n2{20, {}};
    n1.edges.push_back(&n2);
    n2.edges.push_back(&n1);

    std::vector<Node*> G = {&n1, &n2};
    auto encoded = encode(G);
    auto decoded = decode(encoded);

    assert(decoded.size() == 2);
    assert(decoded[0]->data == 10);
    assert(decoded[1]->data == 20);
    assert(decoded[0]->edges.size() == 1);
    assert(decoded[1]->edges.size() == 1);
    assert(decoded[0]->edges[0] == decoded[1].get());
    assert(decoded[1]->edges[0] == decoded[0].get());
}

void test_decode_errors() {
    // Empty input
    try {
        decode({});
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "empty");
    }

    // Negative node count
    try {
        decode({-1});
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "invalid node count");
    }

    // Truncated data
    try {
        decode({1}); // expects data for 1 node
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "truncated data");
    }

    // Truncated degree
    try {
        decode({1, 42}); // has data but no degree
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "truncated degree");
    }

    // Truncated edges
    try {
        decode({1, 42, 2, 0}); // degree=2 but only 1 edge
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "truncated edges");
    }

    // Invalid edge ID
    try {
        decode({1, 42, 1, 5}); // edge points to node 5, but only 1 node
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "invalid edge");
    }

    // Trailing data
    try {
        decode({1, 42, 0, 99}); // extra data at end
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "trailing data");
    }

    // Negative degree
    try {
        decode({1, 42, -1});
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "negative degree");
    }
}

void test_encode_errors() {
    // Null node
    try {
        std::vector<Node*> G = {nullptr};
        encode(G);
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "invalid node");
    }

    // Duplicate node
    try {
        Node n{1, {}};
        std::vector<Node*> G = {&n, &n};
        encode(G);
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "duplicate node");
    }

    // Edge to node not in graph
    try {
        Node n1{1, {}};
        Node n2{2, {}};
        n1.edges.push_back(&n2);
        std::vector<Node*> G = {&n1}; // n2 not in G
        encode(G);
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "invalid node edge");
    }
}

int main() {
    test_single_node();
    test_two_isolated_nodes();
    test_one_edge();
    test_cycle();
    test_empty_graph();
    test_self_loop();
    test_round_trip();
    test_decode_errors();
    test_encode_errors();
    return 0;
}