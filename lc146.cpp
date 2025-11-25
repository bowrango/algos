
class LRUCache {
    // Least-Recently-Used Cache (std::list)
private:
    int capacity;
    list<pair<int,int>> items; // MRU (front), LRU (back)
    unordered_map<int, list<pair<int, int>>::iterator> cache;
public:
    LRUCache(int capacity) {
        this->capacity = capacity;
    }
    
    int get(int key) {
        auto it = cache.find(key);
        if(it == cache.end()) {return -1;}
        // key = it->first
        // node = it->second
        // move node to the front
        items.splice(items.begin(), items, it->second);
        // pair<key, value>.second = value
        return it->second->second;
    }
    
    void put(int key, int value) {
        auto it = cache.find(key);
        if(it != cache.end()) {
            // update existing node
            it->second->second = value;
            items.splice(items.begin(), items, it->second);
        } else {
            if(cache.size() == capacity) {
                // evict
                int lrukey = items.back().first;
                items.pop_back();
                cache.erase(lrukey);
            }
            // add new node
            items.emplace_front(key, value);
            cache[key] = items.begin();
        }
    }
};

// class LRUCache {
//     // Least-Recently-Used Cache (custom pointer logic)
// private:
//     // double-linked list
//     struct Node {
//         int key;
//         int value;
//         Node* prev;
//         Node* next;
//         Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
//     };
//     int capacity;
//     unordered_map<int, Node*> mp;
//     Node* head; // MRU
//     Node* tail; // LRU

//     void addToFront(Node* n) {
//         if(head==nullptr && tail==nullptr) {
//             // first node
//             head = n;
//             tail = n;
//             n->prev = nullptr;
//             n->next = nullptr;
//         } else {
//             // update head
//             n->next = head;
//             n->prev = nullptr;
//             head->prev = n;
//             head = n;
//         }
//     }

//     void removeNode(Node* n) {
//         if(!n) {return;}
//         if(n==head && n==tail) {
//             // only node
//             head = nullptr;
//             tail = nullptr;
//         }
//         else if(n==head) {
//             head = n->next;
//             if(head) {head->prev = nullptr;}
//         }
//         else if(n==tail) {
//             tail = n->prev;
//             if(tail) {tail->next = nullptr;}
//         }
//         else {
//             // middle node
//             Node* left = n->prev;
//             Node* right = n->next;
//             if(left) {left->next = right;}
//             if(right) {right->prev = left;}
//         }
//         // disconnect
//         n->prev = nullptr;
//         n->next = nullptr;
//     }

//     void moveToFront(Node* n) {
//         if(n != head) {
//             removeNode(n);
//             addToFront(n);
//         }
//     }

//     void evict() {
//         if(!tail) {return;}
//         Node* n = tail;
//         removeNode(n);
//         mp.erase(n->key);
//         delete n;
//     }

// public:
//     LRUCache(int capacity) {
//         this->capacity = capacity;
//         head = nullptr;
//         tail = nullptr;
//     }
    
//     int get(int key) {
//         if(mp.count(key) == 0) {return -1;}
//         Node* node = mp[key];
//         moveToFront(node);
//         return node->value;
//     }
    
//     void put(int key, int value) {
//         if(capacity==0) {return;}
//         if(mp.count(key) > 0) {
//             // update Node value
//             Node* node = mp[key];
//             node->value = value;
//             moveToFront(node);
//         } else {
//             // add new Node
//             if(mp.size() >= capacity) {
//                 evict();
//             }
//             Node* n = new Node(key, value);
//             addToFront(n);
//             mp[key] = n;
//         }
//     }
// };