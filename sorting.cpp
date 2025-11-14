
// g++ sorting.cpp -o sorting -std=c++17

#include <vector>
#include <iostream>

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
    std::vector<int> arr = {12, 11, 13, 5, 6, 7, 1, 5, 3, 8};

    for(auto e : heapsort(arr)) {
        std::cout << e << std::endl; 
    }

    quicksort(arr, 0, arr.size()-1);
    for(auto e : arr) {
        std::cout << e << std::endl; 
    }
    return 0;
}