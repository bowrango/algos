// graphscheduler.cpp
// Build: g++ -O2 -std=c++17 graphscheduler.cpp -o greedy
// Run:   ./greedy

// 

// Memory Optimizations
// Operator Fusion
// Operator Legalization
// On-Demand vs. Prefetching

// Compute Optimizations:
// Virtual Array Partitioning

// Memory transfer and compute can be scheduled in parallel to "hide" the transfer cost while satisfying the timing contraint of the operator.


#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

using namespace std;

struct Tensor {
    int id = -1;
    size_t bytes = 0;
    int producer = -1;        // -1 = constant (pre-existing in slow memory)
    vector<int> consumers;    // op ids that read this tensor
};

struct Op {
    int id = -1;
    string name;
    vector<int> inputs;   // tensor ids
    int output = -1;      // tensor id
};

struct MatMulSpec {
    string name;
    int M, K, N;
    // dependencies:
    // (-1)  allocates new constant tensor
    // (>=0) reuses existing tensor id
    int lhs_tensor;
    int rhs_tensor;
    size_t lhs_bytes() const { return (size_t)M*K * sizeof(float); }
    size_t rhs_bytes() const { return (size_t)K*N * sizeof(float); }
    size_t out_bytes() const { return (size_t)M*N * sizeof(float); }
};

struct Scheduler {
    // Schedule ops onto a two-tier memory system:
    //   FAST = limited (SRAM/L2/HBM)
    //   SLOW = unlimited (DRAM/Host)
    // Every load and store increases `moved`, which tracks total movement to be minimized
    size_t cap;              // memory capacity (bytes)
    size_t used = 0;         // memory usage (bytes)
    size_t moved = 0;        // total movement (bytes)
    // Op states
    vector<Op> ops;
    vector<int> remaining_prod;   // number of waiting producers per op
    vector<bool> executed;   // if the op has been executed
    // Tensor states
    vector<Tensor> tensors;
    vector<bool> alive;          // if tensor was produced but not released
    vector<bool> in_fast;        // if tensor lives in fast memory
    vector<int> remaining_con;  // number of waiting consumers per tensor

    Scheduler(size_t cap, vector<Op> ops_, vector<Tensor> tens_)
        : cap(cap), ops(std::move(ops_)), tensors(std::move(tens_))
    {
        remaining_prod.assign(ops.size(), 0);
        executed.assign(ops.size(), false);
        alive.assign(tensors.size(), false);
        in_fast.assign(tensors.size(), false);
        remaining_con.assign(tensors.size(), 0);
        for (auto& t : tensors) {
            remaining_con[t.id] = (int)t.consumers.size();
            if (t.producer < 0) alive[t.id] = true;  // constants start alive in slow
        }
        for (auto& op : ops) {
            for (int tid : op.inputs)
                if (tensors[tid].producer >= 0) remaining_prod[op.id]++;
        }
    }

    int next(int tid) const {
        // find earliest/lowest unscheduled consumer of tensor tid
        // returns -1 if no unscheduled consumers remain
        int best = -1;
        for (int c : tensors[tid].consumers)
            if (!executed[c] && (best < 0 || c < best)) best = c;
        return best;
    }

    void evict_until(size_t need) {
        // evict tensors from fast memory until `need` bytes are available
        while (used + need > cap) {
            int victim = -1;
            // {next consumer id, bytesize} — higher = better eviction candidate
            pair<int,size_t> best_key = {std::numeric_limits<int>::min(), 0};
            // latest/highest unscheduled tensors are better for eviction
            for (int i = 0; i < (int)tensors.size(); i++) {
                if (!alive[i] || !in_fast[i] || remaining_con[i] == 0) continue;
                pair<int,size_t> key = {next(i), tensors[i].bytes};
                if (key > best_key) { best_key = key; victim = i; }
            }
            if (victim < 0) break;
            moved += tensors[victim].bytes;
            used -= tensors[victim].bytes;
            in_fast[victim] = false;
        }
    }

    void load(int tid) {
        // load tensor tid into fast memory if possible
        if (!alive[tid] || in_fast[tid]) return;
        evict_until(tensors[tid].bytes);
        // always incur movement cost
        moved += tensors[tid].bytes;
        if (used + tensors[tid].bytes <= cap) {
            // success
            used += tensors[tid].bytes;
            in_fast[tid] = true;
        }
    }

    void allocate(int tid) {
        // place new tensor tid in fast memory if possible
        evict_until(tensors[tid].bytes);
        if (used + tensors[tid].bytes <= cap) {
            // goes to fast memory
            used += tensors[tid].bytes;
            in_fast[tid] = true;
        } else {
            // goes to slow memory
            moved += tensors[tid].bytes;
            in_fast[tid] = false;
        }
    }

    void free(int tid) {
        // free tensor tid from fast memory if possible
        if (!alive[tid] || remaining_con[tid] != 0) return;
        if (in_fast[tid]) {
            used -= tensors[tid].bytes;
            in_fast[tid] = false;
        }
        alive[tid] = false;
    }

    void exec(int opid) {
        // execute the opid: load inputs, allocate output, decrement completed ops
        auto& op = ops[opid];
        for (int tid : op.inputs) load(tid);
        int out = op.output;
        alive[out] = true;
        allocate(out);
        
        // decrement number of consumers and producers
        for (int tid : op.inputs) {
            remaining_con[tid]--;
            free(tid);
        }
        executed[opid] = true;
        for (int c : tensors[out].consumers)
            remaining_prod[c]--;
    }

    double score(int opid) {
        // score the opid to decide execution order
        auto& op = ops[opid];
        double s = 0;
        for (int tid : op.inputs) {
            if (!alive[tid]) continue;
            // reward/penalize load from fast/slow memory respectively
            s += in_fast[tid] ? (double)tensors[tid].bytes : -(double)tensors[tid].bytes;
            if (remaining_con[tid] == 1 && in_fast[tid]) {
                // reward freeing tid from fast memory
                s += (double)tensors[tid].bytes;
            }
        }
        size_t outb = tensors[op.output].bytes;
        if (used + outb > cap) {
            // penalize overflow
            s -= (double)(used + outb - cap);
        }
        return s;
    }

    pair<vector<int>, size_t> run(bool greedy) {
        // determine execution order using scored topological sort
        
        auto cmp = [&](int a, int b) {
            // max-heap by score
            if (greedy) {return score(a) < score(b);}
            // min-heap by id
            return a > b;
        };

        priority_queue<int, vector<int>, decltype(cmp)> ready(cmp);
        vector<int> order;

        for (auto& op : ops)
            if (remaining_prod[op.id] == 0) ready.push(op.id);

        while (!ready.empty()) {
            // rebuild because scores are updated each execution
            if (greedy) {
                vector<int> tmp;
                while (!ready.empty()) { tmp.push_back(ready.top()); ready.pop(); }
                for (int id : tmp) ready.push(id);
            }
            int pick = ready.top();
            ready.pop();
            order.push_back(pick);
            exec(pick);

            // enqueue newly ready ops (all producers scheduled)
            int out = ops[pick].output;
            for (int c : tensors[out].consumers)
                if (remaining_prod[c] == 0 && !executed[c])
                    ready.push(c);
        }
        return {order, moved};
    }
};

static string fbytes(size_t b) {
    const char* units[] = {"B","KB","MB","GB"};
    double x = (double)b;
    int u = 0;
    while (x >= 1024.0 && u < 3) { x /= 1024.0; ++u; }
    char buf[32];
    snprintf(buf, sizeof(buf), u == 0 ? "%.0f%s" : "%.2f%s", x, units[u]);
    return buf;
}
static void connect(vector<Tensor>& T, int tid, int opid) {
    T[tid].consumers.push_back(opid);
}
static pair<vector<Op>, vector<Tensor>> build_matmul_graph(const vector<MatMulSpec>& specs) {

    // Initialize tensors
    vector<Tensor> tensors;
    int next_tid = 0;
    auto add_tensor = [&](size_t bytes, int producer) {
        int id = next_tid++;
        tensors.push_back({id, bytes, producer, {}});
        return id;
    };

    // Build dependencies
    vector<Op> ops;
    for (int i = 0; i < (int)specs.size(); i++) {
        auto& s = specs[i];
        int opid = (int)ops.size();
        // Allocate new input tensor if one is not reused
        int lhs = s.lhs_tensor >= 0 ? s.lhs_tensor : add_tensor(s.lhs_bytes(), -1);
        int rhs = s.rhs_tensor >= 0 ? s.rhs_tensor : add_tensor(s.rhs_bytes(), -1);
        int out = add_tensor(s.out_bytes(), opid);
        // opid depends on lhs and rhs to return out
        ops.push_back({opid, s.name, {lhs, rhs}, out});
        connect(tensors, lhs, opid);
        connect(tensors, rhs, opid);
    }

    cout << "  tensors:\n";
    for (auto& t : tensors)
        cout << "    t" << t.id << ": " << fbytes(t.bytes)
             << (t.producer < 0 ? " (const)" : " (from " + ops[t.producer].name + ")") << "\n";
    cout << "  ops:\n";
    for (auto& op : ops)
        cout << "    " << op.name << ": t" << op.inputs[0]
             << " @ t" << op.inputs[1] << " -> t" << op.output << "\n";

    return {ops, tensors};
}


static void compare(const vector<Op>& ops, const vector<Tensor>& tensors, size_t cap) {
    Scheduler s1(cap, ops, tensors);
    auto [naive_order, naive_moved] = s1.run(false);
    Scheduler s2(cap, ops, tensors);
    auto [greedy_order, greedy_moved] = s2.run(true);

    auto print = [&](const string& tag, const vector<int>& order, size_t moved) {
        cout << "  " << tag << " order: ";
        for (int id : order) cout << ops[id].name << " ";
        cout << "\n  " << tag << " moved: " << fbytes(moved) << "\n";
    };
    print("naive ", naive_order, naive_moved);
    print("greedy", greedy_order, greedy_moved);
    if (naive_moved > greedy_moved)
        cout << "  savings: " << fbytes(naive_moved - greedy_moved) << "\n";
}

static void test_chain_of_matmuls() {
    cout << "\n=== test two matmuls feeding a third ===\n";
    size_t cap = 2 * 1024 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 512, 1024, 256, -1, -1},
        {"mm1", 256,  512, 128, -1, -1},
        {"mm2", 512,  256, 128,  2,  4},
    });
    compare(ops, tensors, cap);
}

static void test_shared_weight_chain() {
    cout << "\n=== test shared weight chain ===\n";
    size_t cap = 1024 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 128, 512, 512, -1, -1},  // X @ W -> H0
        {"mm1", 128, 512, 512,  2,  1},  // H0 @ W -> H1
        {"mm2", 128, 512, 512,  3,  1},  // H1 @ W -> H2
        {"mm3", 128, 512,  64,  4, -1},  // H2 @ V -> Y
    });
    compare(ops, tensors, cap);
}

static void test_branching_matmuls() {
    cout << "\n=== test branching from shared input ===\n";
    size_t cap = 2 * 1024 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 256, 1024, 512, -1, -1},  // X @ A -> L
        {"mm1", 256, 1024,  64,  0, -1},  // X @ B -> R  (reuses X=t0)
        {"mm2", 256,  512,  64,  2, -1},  // L @ W -> Y
    });
    compare(ops, tensors, cap);
}

// Single op, no dependencies — trivial scheduling
static void test_single_op() {
    cout << "\n=== test single op ===\n";
    size_t cap = 1024 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 64, 64, 64, -1, -1},
    });
    compare(ops, tensors, cap);
}

// Tiny capacity forces all allocations to slow memory and exercises
// the allocate-to-slow path, load failure path, and evict_until no-victim break.
static void test_tiny_memory() {
    cout << "\n=== test tiny memory (forces slow spills) ===\n";
    // capacity smaller than any single tensor
    size_t cap = 4;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 128, 256, 128, -1, -1},
        {"mm1", 128, 128,  64,  2, -1},
    });
    compare(ops, tensors, cap);
}

// Diamond: two independent ops whose outputs feed a third.
// Exercises remaining_prod counting with two producers.
static void test_diamond_dag() {
    cout << "\n=== test diamond DAG ===\n";
    size_t cap = 2 * 1024 * 1024;
    // mm0: A @ B -> t2
    // mm1: C @ D -> t5
    // mm2: t2 @ t5 -> t6   (depends on both mm0 and mm1)
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 128, 256, 128, -1, -1},
        {"mm1", 128, 256, 128, -1, -1},
        {"mm2", 128, 128, 128,  2,  5},
    });
    compare(ops, tensors, cap);
}

// Fan-out: one constant tensor consumed by many ops.
// Exercises eviction scoring with many consumers and remaining_con tracking.
static void test_fan_out() {
    cout << "\n=== test fan-out (shared input) ===\n";
    size_t cap = 512 * 1024;
    // t0 = shared constant input (lhs of all ops)
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 128, 256, 64,  -1, -1},  // t0 @ t1 -> t2
        {"mm1", 128, 256, 64,   0, -1},  // t0 @ t3 -> t4
        {"mm2", 128, 256, 64,   0, -1},  // t0 @ t5 -> t6
        {"mm3", 128, 256, 64,   0, -1},  // t0 @ t7 -> t8
    });
    compare(ops, tensors, cap);
}

// Tight memory: capacity fits only ~2 tensors, forcing heavy eviction
// in a chain. Exercises evict_until with multiple evictions per step.
static void test_tight_eviction() {
    cout << "\n=== test tight memory eviction ===\n";
    // Each tensor is 128*128*4 = 64KB. Cap fits ~2 tensors.
    size_t cap = 128 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 128, 128, 128, -1, -1},
        {"mm1", 128, 128, 128,  2, -1},
        {"mm2", 128, 128, 128,  4, -1},
        {"mm3", 128, 128, 128,  6, -1},
    });
    compare(ops, tensors, cap);
}

// Deep linear chain: each op depends on the previous.
// Exercises sequential scheduling and tensor lifetime/freeing.
static void test_deep_chain() {
    cout << "\n=== test deep chain ===\n";
    size_t cap = 512 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 64, 128, 64, -1, -1},  // t0 @ t1 -> t2
        {"mm1", 64,  64, 64,  2, -1},  // t2 @ t3 -> t4
        {"mm2", 64,  64, 64,  4, -1},  // t4 @ t5 -> t6
        {"mm3", 64,  64, 64,  6, -1},  // t6 @ t7 -> t8
        {"mm4", 64,  64, 64,  8, -1},  // t8 @ t9 -> t10
    });
    compare(ops, tensors, cap);
}

// Exact-fit capacity: memory exactly matches what's needed,
// testing boundary conditions in evict_until and allocate.
static void test_exact_fit() {
    cout << "\n=== test exact fit capacity ===\n";
    // Two inputs + one output = 3 * 32KB = 96KB. Cap = 96KB exactly.
    size_t cap = 3 * 32 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 64, 128, 64, -1, -1},  // lhs=32KB, rhs=32KB, out=16KB
    });
    compare(ops, tensors, cap);
}

// Large tensors: exercises fbytes formatting at MB/GB scale.
static void test_large_tensors() {
    cout << "\n=== test large tensors (fbytes coverage) ===\n";
    size_t cap = (size_t)4 * 1024 * 1024 * 1024;  // 4 GB
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8192, 8192, 8192, -1, -1},
    });
    compare(ops, tensors, cap);
}

// Two independent ops with no shared tensors
static void test_independent_ops() {
    cout << "\n=== test independent ops ===\n";
    size_t cap = 2 * 1024 * 1024;
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 128, 256, 128, -1, -1},
        {"mm1", 64, 128, 64, -1, -1},
    });
    compare(ops, tensors, cap);
}

// Two interleaved weight-sharing chains under tight memory.
// Naive ID-order alternates between groups, thrashing large shared weights.
// Greedy batches each group, loading each weight once.
static void test_interleaved_weight_thrashing() {
    cout << "\n=== test interleaved weight thrashing ===\n";
    size_t cap = 768 * 1024;  // fits one 512KB weight + one small tensor

    // mm0: X_A(t0,128K) @ W_A(t1,512K) -> t2(256K)   group A
    // mm1: X_B(t3,128K) @ W_B(t4,512K) -> t5(256K)   group B
    // mm2: t2 @ W_A(t1) -> t6(256K)                   group A reuses W_A
    // mm3: t5 @ W_B(t4) -> t7(256K)                   group B reuses W_B
    // mm4: t6 @ W_A(t1) -> t8(256K)                   group A reuses W_A
    // mm5: t7 @ W_B(t4) -> t9(256K)                   group B reuses W_B
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 128, 256, 512, -1, -1},   // group A
        {"mm1", 128, 256, 512, -1, -1},   // group B
        {"mm2", 128, 512, 512,  2,  1},   // group A, reuses W_A
        {"mm3", 128, 512, 512,  5,  4},   // group B, reuses W_B
        {"mm4", 128, 512, 512,  6,  1},   // group A, reuses W_A
        {"mm5", 128, 512, 512,  7,  4},   // group B, reuses W_B
    });
    compare(ops, tensors, cap);
}

int main() {
    test_chain_of_matmuls();
    cout << "----\n";
    test_shared_weight_chain();
    cout << "----\n";
    test_branching_matmuls();
    cout << "----\n";
    test_single_op();
    cout << "----\n";
    test_tiny_memory();
    cout << "----\n";
    test_diamond_dag();
    cout << "----\n";
    test_fan_out();
    cout << "----\n";
    test_tight_eviction();
    cout << "----\n";
    test_deep_chain();
    cout << "----\n";
    test_exact_fit();
    cout << "----\n";
    test_large_tensors();
    cout << "----\n";
    test_independent_ops();
    cout << "----\n";
    test_interleaved_weight_thrashing();
    cout << "\nDone.\n";
}
