#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>

// multiple producers write tail and read head. They must reserve slots.
// single consumer writes head and reads tail.

// g++ mpsc_queue.cpp -o mpsc_queue -std=c++17

template<typename T, size_t capacity>
class Queue {
    private:

    struct Slot {
        std::atomic<bool> ready{false};
        T value;
    };
    std::array<Slot, capacity> buffer;
    // monotonic counters
    std::atomic<size_t> head{0};
    std::atomic<size_t> tail{0};
    
    public:
    bool push(const T& item) { // producer
        std::size_t this_tail = tail.load(std::memory_order_relaxed);
        while(true) {
            std::size_t this_head = head.load(std::memory_order_acquire);
            if(this_tail - this_head >= capacity) {
                return false; // full
            }
            if(tail.compare_exchange_weak(this_tail, this_tail+1, 
            std::memory_order_relaxed, std::memory_order_relaxed)) {
                // reserved so tail = this_tail+1
                size_t idx = this_tail % capacity;
                buffer[idx].value = item;
                buffer[idx].ready.store(true, std::memory_order_release);
                return true;
            }
        }
    }

    bool pop(T& item) { // consumer
        std::size_t this_head = head.load(std::memory_order_relaxed);
        Slot& slot = buffer[this_head % capacity];
        if(!slot.ready.load(std::memory_order_acquire)) {
            return false; // empty
        }
        item = slot.value;
        slot.ready.store(false, std::memory_order_relaxed);
        head.store(this_head+1, std::memory_order_release);
        return true;
    }

    size_t size() {
        return tail - head;
    }
};

using T = uint64_t;
constexpr size_t capacity = 1024;

void do_work(Queue<T, capacity>& q) {
    for(int i = 0; i < 5; ++i) {
        q.push(i);
    }
}

int main() {

    Queue<T, capacity> q;

    std::thread a(do_work, std::ref(q));
    std::thread b(do_work, std::ref(q));

    a.join();
    b.join();

    T item;
    q.pop(item);
    std::cout << item << "\n";

    std::cout << q.size() << "\n";

    return 0;
}