class MedianFinder {
private:
    // Use maxheap to store smaller half. Top is the largest of the smallest
    priority_queue<int> maxq;
    // Use minheap to store larger half. Top is the smallest of the largest
    priority_queue<int, vector<int>, greater<int>> minq; 
public:
    MedianFinder() {}
    
    void addNum(int num) {
        // Insert
        if(maxq.empty() || num <= maxq.top()) {
            maxq.push(num);
        }
        else {
            minq.push(num);
        }
        // Rebalance
        if(maxq.size() > minq.size()+1) {
            minq.push(maxq.top()); 
            maxq.pop();
        }
        if(minq.size() > maxq.size()) {
            maxq.push(minq.top());
            minq.pop();
        }
    }

    double findMedian() {
        if((minq.size()+maxq.size())%2==0) { return (minq.top() + maxq.top())/2.0;}
        else{return maxq.top();}
    }
};
