class Solution {
public:
    int videoStitching(vector<vector<int>>& clips, int time) {
    // Linear greedy

    // Compute longest reachable end time from each start time
    vector<int> maxes(time+1, 0);
    for(const auto& clip : clips) {
        if(clip[0] <= time) {
            maxes[clip[0]] = max(maxes[clip[0]], clip[1]);
        }
    }

    int count = 0;
    int left = 0; 
    int right = 0;
    for(int i = 0; i < time; i++) {
        // Greedily take next clip with longest reachable time
        right = max(right, maxes[i]);
        if(i==left) { // [0 left)
            if(right<=i) {return -1;} // Longest reachable time is <= current time. Cannot extend further.
            // Extend coverage
            left = right;
            count++;
        }
    }
    return count;
};
};

