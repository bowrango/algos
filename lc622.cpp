class RingBuffer {
public:
    size_t cap;
    vector<int> q;
    size_t r; // index of next insertion
    size_t l; // index of front element    
    size_t sz;
    RingBuffer(int k) {
        cap=k;
        q.resize(k);
        r=0;
        l=0;
        sz=0;
    }

    int Front() {
        if(isEmpty()) {return -1;}
        return q[l];
    }

    int Rear() {
        if(isEmpty()) {return -1;}
        return q[(r-1+cap)%cap];
    }

    bool enQueue(int value) {
        if(isFull()) {return false;}
        q[r] = value;
        sz++;
        r = (r+1)%cap;
        return true;
    }

    bool deQueue() {
        if(isEmpty()) {return false;}
        sz--;
        l = (l+1)%cap;
        return true;
    }

    bool isFull() {
        return sz==cap;
    }

    bool isEmpty() {
        return sz==0;
    }  
};