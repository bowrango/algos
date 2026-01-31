
// g++ sorting.cpp -o sorting -std=c++17

#include <vector>
#include <iostream>
#include <cassert>
#include <random>

std::vector<int> randn(std::size_t n, int min=0, int max=100) {
    std::random_device rng;
    std::mt19937 gen(rng());
    std::uniform_int_distribution<> dist(min, max);
    std::vector<int> x;
    x.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        x.push_back(dist(gen));
    }
    return x;
}

template <typename T>
class MinHeap {
    private:
        std::vector<T> heap;
        int parent(int idx) {return (idx - 1) / 2;}
        int left(int idx) {return 2*idx + 1;}
        int right(int idx) {return 2*idx + 2;}
        void heapup(int idx) {
            while (idx && heap[idx] < heap[parent(idx)]) {
                std::swap(heap[idx], heap[parent(idx)]);
                idx = parent(idx);
            }
        }
        void heapdown(int idx) {
            int small = idx;
            int l = left(idx);
            int r = right(idx);
            if (l < heap.size() && heap[l] < heap[small]) {
                small = l;
            }
            if (r < heap.size() && heap[r] < heap[small]) {
                small = r;
            }
            if (small != idx) {
                std::swap(heap[idx], heap[small]);
                heapdown(small);
            }
        }
        void build() {
            // heapdown on internal nodes from bottom to root
            for (int i = heap.size()/2 - 1; i >= 0; --i) {
                heapdown(i);
            }
        }
    public:
    // constructors
    MinHeap() {}
    MinHeap(std::vector<T>& x) : heap(x) {
        build();
    }
    // methods
    bool isempty() const {return heap.empty();}
    int size() const {return heap.size();}
    int min() const {
        // Return lowest element without removal
        //std::assert(!isempty());
        return heap.front();
    }     
    void insert(T key) {
        heap.push_back(key);
        int idx = heap.size() - 1;
        heapup(idx);
    }
    T pop() {
        // Return lowest element with removal
        //std::assert(!isempty());
        // Store root (the minimum) as temp variable
        T root = heap.front();
        // Replace root with last array element
        heap[0] = heap.back();
        // Remove last array element
        heap.pop_back();
        // Rebuild heap from root
        heapdown(0);
        return root;
    }
    void remove(T key) {
        // Delete element at key
        // Find idx of the key
        int idx = -1;
        for (int i = 0; i < heap.size(); ++i) {
            if (heap[i] == key) {
                idx = i;
            }
        }
        //std::assert(idx!=-1);
        // Replace with last element
        heap[idx] = heap.back();
        heap.pop_back();
        // Rebuild entire heap
        heapup(idx);
        heapdown(idx);
    }
    void decrease(int idx, T key) {
        //std::assert(0 <= idx && idx < heap.size());
        //std::assert(key < heap[idx]);
        heap[idx] = key;
        heapup(idx);
    }
};

void quicksort(std::vector<int>& x, int lo, int hi) {
    
    int i, j, pivot;
    if (lo < hi) {
        pivot = lo;
        i = lo;
        j = hi;
        while (i < j) {
            // Move i,j towards each other to swap the out-of-place pair 
            while (i <= hi && x[i] <= x[pivot]) {++i;}
            while (j >= lo && x[j] > x[pivot]) {--j;}
            if (i < j) {std::swap(x[i], x[j]);}
        }
        // j is now the correct pivot so swap it
        std::swap(x[pivot], x[j]);
        // Recurse on the two partitions
        quicksort(x, lo, j-1);
        quicksort(x, j+1, hi);
    }
}

std::vector<int> myheapsort(std::vector<int> x) {
    MinHeap<int> heap(x);
    std::vector<int> y;
    y.reserve(x.size());
    while(!heap.isempty()) {
        y.push_back(heap.pop());
    }
    return y;
}

std::vector<int> heapsort(std::vector<int> x) {
    // O(n + nlogn) = O(nlogn)
    // build binary max-heap: O(n)
    std::make_heap(x.begin(), x.end()); 
    for (auto end = x.end(); end != x.begin(); --end) { // n iterations
        // Swap largest element with the final element
        // O(logn) := height of the tree
        std::pop_heap(x.begin(), end);
    }
    return x;
}

int main() {
    
    std::vector<int> arr = randn(100);

    auto x = heapsort(arr);
    auto y = myheapsort(arr);

    assert(y==x);

    // quicksort(arr, 0, arr.size()-1);
    // for(auto e : arr) {
    //     std::cout << e << std::endl; 
    // }

    return 0;
}