#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>

// multiple producers write tail and read head. They must reserve slots.
// multiple consumers write tail and read head. They must reserve slots.

template<typename T, std::size_t capacity>
class Queue {
    private:
    struct Slot {
        T value;
        std::atomic<std::size_t> seq;
    };
    std::array<Slot, capacity> buffer;
    std::atomic<std::size_t> head{0};
    std::atomic<std::size_t> tail{0};
    public:
    Queue() {
        for(std::size_t i=0; i < capacity; ++i) {
            buffer[i].seq.store(i, std::memory_order_relaxed);
        }
    }
    bool push(const T& item) {
        std::size_t this_tail = tail.load(std::memory_order_relaxed);
        while(true) {
            Slot& slot = buffer[this_tail % capacity];
            std::size_t seq = slot.seq.load(std::memory_order_acquire);
            if(this_tail == seq) {
                // slot is free
                if(tail.compare_exchange_weak(this_tail, this_tail+1, 
                std::memory_order_relaxed, std::memory_order_relaxed)) {
                    // reserved
                    buffer[this_tail % capacity].value = item;
                    slot.seq.store(this_tail+1, std::memory_order_release);
                    return true;
                }
            } else if (this_tail < seq) {
                this_tail = tail.load(std::memory_order_relaxed);
            } else {
                return false; // full
            }
        }
    }

    bool pop(T& item) {
        std::size_t this_head = head.load(std::memory_order_relaxed);
        while(true) {
            Slot& slot = buffer[this_head % capacity];
            std::size_t seq = slot.seq.load(std::memory_order_acquire);
            if(this_head+1 == seq) {
                // slot is free
                if(head.compare_exchange_weak(this_head, this_head+1, 
                std::memory_order_relaxed, std::memory_order_relaxed)) {
                    // reserved
                    item = slot.value;
                    // publish empty slot
                    slot.seq.store(this_head+capacity, std::memory_order_release);
                    return true;
                }
            } else if(seq < this_head+1) {
                return false; // empty
            } else {
                this_head = head.load(std::memory_order_relaxed);
            }
        }
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

    std::cout << q.size() << "\n";

    return 0;
}