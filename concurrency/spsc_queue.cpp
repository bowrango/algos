
#include <atomic>
#include <cstddef>
#include <iostream>

// bounded fixed-array circular queue
// producer/push writes tail and reads head
// consumer/pop writes head and reads tail

// g++ spsc_queue.cpp -o spsc_queue -std=c++17

template<typename T, std::size_t capacity>
class Queue {
    private:
    T buffer[capacity];
    std::atomic<std::size_t> head{0};
    std::atomic<std::size_t> tail{0};
    public:
    bool push(const T& item) { // producer
        std::size_t this_tail = tail.load(std::memory_order_relaxed);
        std::size_t next_tail = (this_tail + 1) % capacity;
        std::size_t this_head = head.load(std::memory_order_acquire);
        if(next_tail == this_head) {
            return false; // full
        }
        buffer[this_tail] = item;
        tail.store(next_tail, std::memory_order_release);
        return true;
    }

    bool pop(T& item) { // consumer
        std::size_t this_head = head.load(std::memory_order_relaxed);
        std::size_t this_tail = tail.load(std::memory_order_acquire);
        if(this_head == this_tail) {
            return false; // empty
        }
        item = buffer[this_head];
        head.store((this_head + 1) % capacity, std::memory_order_release);
        return true;
    }

    size_t size() {
        return (tail + capacity - head) % capacity;
    }
};

int main() {
    using T = uint64_t;
    Queue<T, 1024> q; // [],tail=0,head=0
    q.push(0); // [0],tail=1,head=0
    q.push(1); // [0,1],tail=2,head=0

    T item;
    q.pop(item); // [1],tail=2,head=1
    std::cout << item << "\n"; // 0

    q.push(2); // [1,2],tail=3,head=1
    q.push(3); // [1,2,3],tail=4,head=1

    q.pop(item); // [2,3],tail=4,head=2
    std::cout << item << "\n"; // 1

    std::cout << q.size() << "\n";
    return 0;
}