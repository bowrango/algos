# Data Structures and Algorithms

C++/Python implementations of fundamental algorithms and data structures.

The `leetcode/` directory has solutions to LeetCode challenges and `misc/` contains various other exercises:
- `chimera.cpp` - matmul graph compiler with greedy place/sched to minimize data movement (loosely based on [Chimera Graph Compiler](https://app.quadric.ai/docs/latest/chimera-software-user-guide/chimera-graph-compiler-cgc/overview-of-the-cgc))
- `candybar.cpp` - memoization with fun improvement using matrix exponentiation
- `knapsack.cpp` - binary knapsack with memoization
- `graph_serialize.cpp` - translate between graph of `vector<Node*>` and serial `vector<int>`
- `node_parser.py` - binary tree parser with `regex`
- `dijkstra.cpp` - shortest path with binary heap
- `bench_dijkstra.cpp` - benchmark Dijkstra against baseline, Fibonacci heap, binary tree, and binary heap
- `sorting.cpp` - compare quicksort and heapsort using custom and built-in min-heap
- `trees.cpp` - binary splay tree with inversion
- `objects.cpp` - basic operator overloading

## Build/Run

```C++
g++ <file>.cpp -o <file> -std=c++17
./<file>
```
```Python
python <file>.py
```
