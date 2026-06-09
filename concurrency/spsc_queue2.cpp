#include <thread>
#include <atomic>
#include <memory>

// unbounded linked-list queue
// producer/push writes tail and links a new dummy node
// consumer/pop writes head and deletes old head nodes
// tail always points at an empty dummy node

// g++ spsc_queue2.cpp -o spsc_queue2 -std=c++17

template<typename T>
class Queue {
    private:
    struct node {
        std::shared_ptr<T> data;
        node* next;
        node() : next(nullptr) {}
    };
    std::atomic<node*> head;
    std::atomic<node*> tail;
	    node* pop_head() {
	        node* const oldHead = head.load();
	        if(oldHead == tail.load()) {
	            return nullptr; // empty
	        }
	        // old head contains data; next node becomes the new front
	        head.store(oldHead->next);
	        return oldHead;
	    }
    public:
	    Queue() : head(new node), tail(head.load()) {}
	    Queue(const Queue& other) = delete;
	    Queue& operator=(const Queue& other) = delete;
	    ~Queue() {
	        // drain remaining nodes, including the final dummy node
	        while(node* const oldHead=head.load()) {
	            head.store(oldHead->next);
	            delete oldHead;
	        }
    }
    std::shared_ptr<T> pop() {
        node* oldHead = pop_head();
        if(!oldHead) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(oldHead->data);
        delete oldHead;
        return res;
    }
    void push(T item) {
        std::shared_ptr<T> newData(std::make_shared<T>(item));
        node* p = new node;
        node* const oldTail = tail.load();
        // fill the current dummy tail, link a fresh dummy node after it
        oldTail->data.swap(newData);
        oldTail->next = p;
        // synchronize with the tail.load() in pop_head
    tail.store(p);
    }
};

int main() {
    return 0;
}
