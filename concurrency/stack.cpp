#include <atomic>
#include <memory>
#include <thread>
#include <functional>

// g++ stack.cpp -o stack -std=c++17

// Thread-Safe but not purely lock-free since internally locks are used for atomic operations on shared_ptr. A true lock-free would use std::atomic<node*> head with some memory reclaimation scheme: hazard pointers, reference-counted nodes, etc.

template<typename T>
class Stack {
    private:
    struct node {
        std::shared_ptr<T> data;
        std::shared_ptr<node> next;
        // allocate data_ on the heap
        node(const T& data_) : data(std::make_shared<T>(data_)) {}
    };
    std::shared_ptr<node> head;
    public:
    void push(const T& data) {
        std::shared_ptr<node> const newNode = std::make_shared<node>(data);
        newNode->next = std::atomic_load(&head);
        // ensure the head pointer still has the same value as newNode->next
        while(!std::atomic_compare_exchange_weak(&head, &newNode->next, newNode));
    }
    std::shared_ptr<T> pop() {
        std::shared_ptr<node> oldHead = std::atomic_load(&head);
        while(oldHead && !std::atomic_compare_exchange_weak(&head,&oldHead,oldHead->next));
        return oldHead ? oldHead->data : std::shared_ptr<T>();
    }
    ~Stack() {
        while(pop());
    }
};

using T = uint64_t;

void do_work(Stack<T>& s) {
    for(int i = 0; i < 5; ++i) {
        s.push(i);
    }
}

int main() {

    Stack<T> s;

    std::thread a(do_work, std::ref(s));
    std::thread b(do_work, std::ref(s));

    a.join();
    b.join();

    return 0;
}