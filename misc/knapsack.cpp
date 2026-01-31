
// g++ knapsack.cpp -o knapsack -std=c++17

#include <iostream>
#include <vector>

int knapsack(int W, std::vector<int>& v, std::vector<int>& w) {
    // O(n*W) Time and O(W) Space

    // dp[j] is the max possible value with total weight j
    std::vector<int> dp(W+1, 0);

    // go forward through items
    for (int i=0; i < w.size(); ++i) {
        // go backwards through capacities
        // this ensures dp[j-w[i]] uses the previous iterate
        // as to not include an item more than once.
        for (int j=W; j >= w[i]; --j) {
            // choose the maximum of:
            // not taking item := dp[j]
            // taking the item := dp[j-w[i]] + v[i]
            // (max previous value for leftover weight) + (item value)
            dp[j] = std::max(dp[j], dp[j-w[i]] + v[i]);
        }
    }
    return dp[W];
}

int main() {
    std::vector<int> v = {1,2,3};
    std::vector<int> w = {4,5,1};
    int W = 4;

    std::cout << knapsack(W,v,w) << std::endl;
}