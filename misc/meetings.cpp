#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>

using namespace std;

struct Meeting {
    int start; // earliest start time
    int end;  // latest finish time
    int equip; // required equipment capacity
};

int schedule(const vector<Meeting>& meetings, const int M, const int K) {
    // fixed capacity M
    // K meeting rooms

   // Filter
   vector<Meeting> valid;
   for(const auto& m : meetings) {
        if(m.equip <= M) {
            valid.push_back(m);
        } 
    }
    if(valid.empty()) {return 0;}

    // Sort by earliest time
    sort(valid.begin(), valid.end(), 
        [](const Meeting& a, const Meeting& b) {return a.end < b.end;}
    );

    // occ[t] = number of meetings scheduled at hour t
    unordered_map<int, int> occ;
    int S = 0;

    // Greedy scheduling
    for(const auto& m : valid) {
        // Try earliest feasible hour in this window
        for(int t = m.start; t < m.end; ++t) {
            if(occ[t] < K) {
                // schedule
                occ[t]++;
                S++;
                break;
            }
        }
    }
    return S;
}


int main() {
    {
        vector<Meeting> meetings = {
            {1, 4, 2},
            {2, 3, 1},
            {2, 5, 4},
            {3, 6, 2}
        };
        cout << schedule(meetings, 2, 1) << endl; // 3
    }

    {
        vector<Meeting> meetings = {
            {1, 2, 1},
            {2, 3, 1},
            {0, 10, 1},
            {0, 10, 1}
        };
        cout << schedule(meetings, 2, 2) << endl; // 4
    }

    {
        vector<Meeting> meetings = {
            {1, 2, 1},
            {2, 3, 1},
            {0, 10, 1},
            {0, 10, 1}
        };
        cout << schedule(meetings, 2, 1) << endl; // 4
    }

    return 0;
}