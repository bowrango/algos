// chimera.cpp
// Build: g++ chimera.cpp -o chimera -std=c++17 
// Run:   ./chimera
//
// Simplified model of Quadric's Chimera Graph Compiler (CGC)
//
// Assumptions:
//   - Accepts any generic matrix-multiply sequence of valid sizes
//   - 2D grid of processing elements (PEs): 8x8 (64), 16x16 (256), or 32x32 (1024).
//   - Each PE has 4KB of fast memory (e.g., SRAM) with unlimited slow memory (e.g., L2) accessible only at grid edge.
//   - Evicted tensors and new tensors that don't fit on PEs overflow to the slow memory.
//   - Greedy placement puts ops on PEs that minimize the input transfer cost.
//   - Greedy scheduling orders ops by how cheaply they run in the current PE.
//
// Not yet modeled:
//   - Operator fusion, legalization, tiling/partitioning
//   - Overlapped compute and DMA (prefetching)
//   - Virtual array partitioning across PEs

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
    int producer = -1;        // -1 = initialized in slow memory
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

struct Compiler {
    // Place and Schedule ops onto a 2D grid of processing elements (PEs):
    //   fast = limited local registers (4KB each)
    //   slow  = unlimited global memory at the boundary
    // Data movement costs bytes * hops.
    // PE-to-PE: bytes * manhattan distance. DRAM: bytes * (distance to nearest boundary + 1).
    // Every load and store increases `moved`, which tracks total movement to be minimized.
    int rows, cols, num_pes;
    size_t pe_cap;               // per-PE register capacity (bytes)
    vector<size_t> pe_used;      // per-PE register usage (bytes)
    size_t moved = 0;            // total weighted transfer (bytes * hops)
    size_t slow_moved = 0;       // subset of moved from DRAM loads/stores
    size_t fast_moved = 0;         // subset of moved from PE-to-PE transfers
    // Op states
    vector<Op> ops;
    vector<int> remaining_prod;  // number of waiting producers per op
    vector<bool> executed;       // if the op has been executed
    vector<int> op_pe;           // which PE each op executed on
    int rr_counter = 0;          // round-robin counter for naive placement
    // Tensor states
    vector<Tensor> tensors;
    vector<bool> alive;          // if tensor was produced but not released
    vector<int> location;        // which PE tensor lives on, -1 = DRAM
    vector<int> remaining_con;   // number of waiting consumers per tensor

    Compiler(int grid_side, vector<Op> ops_, vector<Tensor> tens_,
              size_t pe_cap_ = 4096)
        : rows(grid_side), cols(grid_side), num_pes(grid_side * grid_side),
          pe_cap(pe_cap_),
          ops(std::move(ops_)), tensors(std::move(tens_))
    {
        pe_used.assign(num_pes, 0);
        remaining_prod.assign(ops.size(), 0);
        executed.assign(ops.size(), false);
        op_pe.assign(ops.size(), -1);
        alive.assign(tensors.size(), false);
        location.assign(tensors.size(), -1);
        remaining_con.assign(tensors.size(), 0);
        for (auto& t : tensors) {
            remaining_con[t.id] = (int)t.consumers.size();
            if (t.producer < 0) alive[t.id] = true;  // constants start alive in DRAM
        }
        for (auto& op : ops) {
            for (int tid : op.inputs)
                if (tensors[tid].producer >= 0) remaining_prod[op.id]++;
        }
    }

    int manhattan(int pe_a, int pe_b) const {
        // compute manhattan distance between two PEs
        int r1 = pe_a / cols, c1 = pe_a % cols;
        int r2 = pe_b / cols, c2 = pe_b % cols;
        return abs(r1 - r2) + abs(c1 - c2);
    }

    int dram_hops(int pe) const {
        // compute manhattan distance to nearest grid boundary
        int r = pe / cols, c = pe % cols;
        // +1 for the interface
        return min({r, c, rows - 1 - r, cols - 1 - c}) + 1;
    }

    size_t cost(int tid, int pe) const {
        // compute transfer cost to move tensor tid to pe
        int loc = location[tid];
        if (loc == pe) return 0;
        size_t bytes = tensors[tid].bytes;
        if (loc < 0) return bytes * dram_hops(pe);   // from slow to fast
        return bytes * manhattan(loc, pe);           // from fast to fast
    }

    int next(int tid) const {
        // find earliest/lowest unscheduled consumer of tensor tid
        // returns -1 if no unscheduled consumers remain
        int best = -1;
        for (int c : tensors[tid].consumers)
            if (!executed[c] && (best < 0 || c < best)) best = c;
        return best;
    }

    void evict_until(int pe, size_t need) {
        // evict tensors from pe's register until `need` bytes are available
        while (pe_used[pe] + need > pe_cap) {
            int victim = -1;
            // {next consumer id, bytesize} — higher = better eviction candidate
            pair<int,size_t> best_key = {std::numeric_limits<int>::min(), 0};
            // latest/highest unscheduled tensors are better for eviction
            for (int i = 0; i < (int)tensors.size(); i++) {
                if (!alive[i] || location[i] != pe || remaining_con[i] == 0) continue;
                pair<int,size_t> key = {next(i), tensors[i].bytes};
                if (key > best_key) { best_key = key; victim = i; }
            }
            if (victim < 0) break;
            // evict to slow memory via nearest boundary
            size_t c = tensors[victim].bytes * dram_hops(pe);
            moved += c;
            slow_moved += c;
            pe_used[pe] -= tensors[victim].bytes;
            location[victim] = -1;
        }
    }

    void load(int tid, int pe) {
        // load tensor tid onto pe's registers if possible
        if (!alive[tid] || location[tid] == pe) return;
        int src = location[tid];

        // always incur movement cost
        size_t c = cost(tid, pe);
        moved += c;
        if (src < 0) {slow_moved += c;}
        else {fast_moved += c;}

        // free space on source PE
        if (src >= 0) pe_used[src] -= tensors[tid].bytes;

        evict_until(pe, tensors[tid].bytes);
        if (pe_used[pe] + tensors[tid].bytes <= pe_cap) {
            // success
            pe_used[pe] += tensors[tid].bytes;
            location[tid] = pe;
        } else {
            // goes to slow memory
            location[tid] = -1;
        }
    }

    void allocate(int tid, int pe) {
        // place new tensor tid on pe's registers if possible
        evict_until(pe, tensors[tid].bytes);
        if (pe_used[pe] + tensors[tid].bytes <= pe_cap) {
            // success
            pe_used[pe] += tensors[tid].bytes;
            location[tid] = pe;
        } else {
            // goes to slow memory via nearest boundary
            size_t c = tensors[tid].bytes * dram_hops(pe);
            moved += c;
            slow_moved += c;
            location[tid] = -1;
        }
    }

    void free(int tid) {
        // free tensor tid from PE registers if possible
        if (!alive[tid] || remaining_con[tid] != 0) return;
        if (location[tid] >= 0) {
            pe_used[location[tid]] -= tensors[tid].bytes;
        }
        location[tid] = -1;
        alive[tid] = false;
    }

    void exec(int opid, int pe) {
        // execute the opid on pe: load inputs, allocate output, decrement completed ops
        auto& op = ops[opid];
        for (int tid : op.inputs) load(tid, pe);
        int out = op.output;
        alive[out] = true;
        allocate(out, pe);

        // decrement number of consumers and producers
        for (int tid : op.inputs) {
            remaining_con[tid]--;
            free(tid);
        }
        executed[opid] = true;
        op_pe[opid] = pe;
        for (int c : tensors[out].consumers)
            remaining_prod[c]--;
    }

    int place(int opid) {
        // find the best PE for opid by minimizing total input transfer cost
        // break ties using most free register space, then lowest PE id
        auto& op = ops[opid];
        int best_pe = 0;
        size_t best_cost = std::numeric_limits<size_t>::max();
        size_t best_free = 0;
        for (int p = 0; p < num_pes; p++) {
            size_t c = 0;
            for (int tid : op.inputs)
                c += cost(tid, p);
            size_t free_space = pe_cap - pe_used[p];
            if (c < best_cost ||
               (c == best_cost && free_space > best_free)) {
                best_cost = c;
                best_free = free_space;
                best_pe = p;
            }
        }
        return best_pe;
    }

    double score(int opid) {
        // score the opid to decide execution order
        auto& op = ops[opid];
        int best_pe = place(opid);
        double s = 0;
        for (int tid : op.inputs) {
            if (!alive[tid]) continue;
            // penalize transfer cost to the best PE
            s -= (double)cost(tid, best_pe);
            if (remaining_con[tid] == 1 && location[tid] >= 0) {
                // reward freeing tid from a PE registers
                s += (double)tensors[tid].bytes;
            }
        }
        size_t outb = tensors[op.output].bytes;
        if (pe_used[best_pe] + outb > pe_cap) {
            // penalize overflow
            s -= (double)((pe_used[best_pe] + outb - pe_cap) * dram_hops(best_pe));
        }
        return s;
    }

    pair<vector<int>, size_t> run(bool greedy) {
        // determine execution schedule and placement using scored topological sort

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

            int pe = greedy ? place(pick) : rr_counter++ % num_pes;
            order.push_back(pick);
            exec(pick, pe);

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
    vector<Tensor> tensors;
    int next_tid = 0;
    auto add_tensor = [&](size_t bytes, int producer) {
        int id = next_tid++;
        tensors.push_back({id, bytes, producer, {}});
        return id;
    };
    vector<Op> ops;
    for (int i = 0; i < (int)specs.size(); i++) {
        auto& s = specs[i];
        int opid = (int)ops.size();
        int lhs = s.lhs_tensor >= 0 ? s.lhs_tensor : add_tensor(s.lhs_bytes(), -1);
        int rhs = s.rhs_tensor >= 0 ? s.rhs_tensor : add_tensor(s.rhs_bytes(), -1);
        int out = add_tensor(s.out_bytes(), opid);
        ops.push_back({opid, s.name, {lhs, rhs}, out});
        connect(tensors, lhs, opid);
        connect(tensors, rhs, opid);
    }
    return {ops, tensors};
}

static void compare(const string& label, const vector<Op>& ops, const vector<Tensor>& tensors,
                    int grid_side, size_t pe_cap = 4096) {

    // baseline
    Compiler c1(grid_side, ops, tensors, pe_cap);
    auto [naive_order, naive_moved] = c1.run(false);
    auto& np = c1.op_pe;
    
    // optimized
    Compiler c2(grid_side, ops, tensors, pe_cap);
    auto [greedy_order, greedy_moved] = c2.run(true);
    auto& gp = c2.op_pe;

    auto pe_str = [&](const vector<int>& placement) {
        string s;
        for (int i = 0; i < (int)ops.size(); i++) {
            if (i) s += " ";
            s += ops[i].name + "->(" + to_string(placement[i] / grid_side)
                 + "," + to_string(placement[i] % grid_side) + ")";
        }
        return s;
    };

    cout << label << " [" << grid_side << "x" << grid_side << "]\n";
    cout << "  naive:  " << fbytes(naive_moved) << "  " << pe_str(np) << "\n";
    cout << "  greedy: " << fbytes(greedy_moved) << "  " << pe_str(gp) << "\n";
    if (naive_moved > greedy_moved)
        cout << "  savings: " << fbytes(naive_moved - greedy_moved) << "\n";
}

// ===== Tests =====

static void test_single_op() {
    // trivial: one op, no dependencies
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
    });
    compare("single op", ops, tensors, 8);
}

static void test_chain() {
    // linear chain: each op depends on the previous
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8,  8, 8,  2, -1},
        {"mm2", 8,  8, 8,  4, -1},
    });
    compare("chain", ops, tensors, 8);
}

static void test_deep_chain() {
    // 5-op sequential chain
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8,  8, 8,  2, -1},
        {"mm2", 8,  8, 8,  4, -1},
        {"mm3", 8,  8, 8,  6, -1},
        {"mm4", 8,  8, 8,  8, -1},
    });
    compare("deep chain", ops, tensors, 8);
}

static void test_diamond() {
    // two independent ops whose outputs feed a third
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8, 16, 8, -1, -1},
        {"mm2", 8,  8, 8,  2,  5},
    });
    compare("diamond", ops, tensors, 8);
}

static void test_fan_out() {
    // one shared constant input consumed by 4 ops
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8, 16, 8,  0, -1},
        {"mm2", 8, 16, 8,  0, -1},
        {"mm3", 8, 16, 8,  0, -1},
    });
    compare("fan-out", ops, tensors, 8);
}

static void test_shared_weight_chain() {
    // weight W reused across a chain: mm0, mm1, mm2 all read W
    // W=8*16*4=512B, fits in 4KB — greedy keeps it resident
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},  // t0 @ t1(W) -> t2
        {"mm1", 8,  8, 8,  2,  1},  // t2 @ t1(W) -> t3
        {"mm2", 8,  8, 8,  3,  1},  // t3 @ t1(W) -> t4
        {"mm3", 8,  8, 4,  4, -1},  // t4 @ t5(V) -> t6
    });
    compare("shared weight chain", ops, tensors, 8);
}

static void test_branching() {
    // branching from a shared input: X feeds two parallel ops
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},   // X @ A -> L
        {"mm1", 8, 16, 4,  0, -1},   // X @ B -> R
        {"mm2", 8,  8, 4,  2, -1},   // L @ W -> Y
    });
    compare("branching", ops, tensors, 8);
}

static void test_two_clusters() {
    // two independent subgraphs, no shared tensors
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8,  8, 8,  2, -1},
        {"mm2", 8, 16, 8, -1, -1},
        {"mm3", 8,  8, 8,  7, -1},
    });
    compare("two clusters", ops, tensors, 8);
}

static void test_independent_ops() {
    // fully independent ops, all ready at once
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8, 16, 4, -1, -1},
    });
    compare("independent ops", ops, tensors, 8);
}

static void test_tight_registers() {
    // pe_cap=1024B, each tensor ~256B — forces heavy eviction per step
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 8, 8, -1, -1},   // lhs=256B, rhs=256B, out=256B
        {"mm1", 8, 8, 8,  2, -1},
        {"mm2", 8, 8, 8,  4, -1},
        {"mm3", 8, 8, 8,  6, -1},
    });
    compare("tight registers", ops, tensors, 8, 1024);
}

static void test_exact_fit() {
    // pe_cap exactly fits two inputs + output: 512+512+256=1280B
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},  // lhs=512B, rhs=512B, out=256B
    });
    compare("exact fit", ops, tensors, 8, 1280);
}

static void test_dram_fallback() {
    // tensors > pe_cap, everything goes through DRAM
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 64, 64, 64, -1, -1},  // 16KB per tensor >> 4KB pe_cap
        {"mm1", 64, 64, 64,  2, -1},
    });
    compare("overflow", ops, tensors, 8);
}

static void test_large_grid() {
    // same chain on 32x32 — naive round-robin spreads across distant PEs
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8,  8, 8,  2, -1},
        {"mm2", 8,  8, 8,  4, -1},
    });
    compare("large grid", ops, tensors, 32);
}

static void test_interleaved_weight_thrashing() {
    // two groups sharing large weights, interleaved by op id
    // naive alternates groups, thrashing weights; greedy batches each group
    // lhs=256B, rhs(W)=1024B, out=512B — W dominates
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 8, 32, -1, -1},    // group A: X_A @ W_A -> t2
        {"mm1", 8, 8, 32, -1, -1},    // group B: X_B @ W_B -> t5
        {"mm2", 8, 32, 32,  2,  1},   // group A: t2 @ W_A -> t6
        {"mm3", 8, 32, 32,  5,  4},   // group B: t5 @ W_B -> t7
        {"mm4", 8, 32, 32,  6,  1},   // group A: t6 @ W_A -> t8
        {"mm5", 8, 32, 32,  7,  4},   // group B: t7 @ W_B -> t9
    });
    compare("interleaved thrashing", ops, tensors, 8);
}

static void test_fan_out_16x16() {
    // fan-out on a larger grid to test placement clustering
    auto [ops, tensors] = build_matmul_graph({
        {"mm0", 8, 16, 8, -1, -1},
        {"mm1", 8, 16, 8,  0, -1},
        {"mm2", 8, 16, 8,  0, -1},
        {"mm3", 8, 16, 8,  0, -1},
    });
    compare("fan-out", ops, tensors, 16);
}

int main() {
    test_single_op();
    test_chain();
    test_deep_chain();
    test_diamond();
    test_fan_out();
    test_shared_weight_chain();
    test_branching();
    test_two_clusters();
    test_independent_ops();
    test_tight_registers();
    test_exact_fit();
    test_dram_fallback();
    test_large_grid();
    test_interleaved_weight_thrashing();
    test_fan_out_16x16();
    return 0;
}
