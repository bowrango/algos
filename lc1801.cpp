class Solution {
public:
    bool cross(const bool side, const int ours, const int other) {
        if(side) {
            // match sell order
            return ours <= other;
        } else {
            // match buy order
            return ours >= other;
        }
    } 
    void place(const int price, int& amount, const bool side, auto& left, auto& right, auto& volume) {
        while(!right.empty() && amount > 0) {
            auto [px, qty] = right.top();
            if(!cross(side, price, px)) {break;}
            right.pop(); 
            int filled = min(amount, qty);
            amount -= filled;
            qty -= filled;
            volume -= filled;
            if(qty > 0) {
                // backlog their extra order
                right.push({px, qty});
            }
        }
        if(amount > 0) {
            // backlog our extra order
            left.push({price, amount});
            volume += amount;
        }
    }
    int getNumberOfBacklogOrders(vector<vector<int>>& orders) {

        const int MOD = 1e9 + 7;
        long long total = 0;
        // maxheap (price, amount)
        priority_queue<pair<int,int>> buys; 
        // minheap (price, amount)
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> sells;
        bool side;
        for(auto& order : orders) {
            side = order[2];
            if(order[2]) {
                // sell
                place(order[0], order[1], side, sells, buys, total);
            } else {
                // buy
                place(order[0], order[1], side, buys, sells, total);
            }
        }
        return total % MOD;
    }
};
