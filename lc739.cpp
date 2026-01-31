class Solution {
public:
    vector<int> dailyTemperatures(vector<int>& temps) {
    const int n = temps.size();
    vector<int> days(n,0);
    // Store day indices of increasing temperature from bottom to top
    stack<int> s;
    for (int i=n-1; i >= 0; i--) {
        // Remove days less than the current temperature
        while (!s.empty() && temps[s.top()] <= temps[i]) {s.pop();}
        if (!s.empty()) {days[i] = s.top() - i;}
        s.push(i);
    }    
    return days;
};
};