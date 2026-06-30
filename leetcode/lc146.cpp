// LC146: LRU Cache
// Least-Recently-Used cache with O(1) get/put using doubly linked list + hash map.

class LRUCache {
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
            // add new node
            if(cache.size() == capacity) {
                // evict least-recently-used
                int lrukey = items.back().first;
                items.pop_back();
                cache.erase(lrukey);
            }
            items.emplace_front(key, value);
            cache[key] = items.begin();
        }
    }
};

// class LRUCache {
// private:
// struct Node {
//     int key;
//     int value;
//     Node* right;
//     Node* left;
//     Node(int k, int v) : key(k), value(v), right(nullptr), left(nullptr) {}
// };
// int capacity;
// // lookup[key] = pointer to node
// std::unordered_map<int, Node*> lookup;
// Node* head;
// Node* tail;

// void addToFront(Node* n) {
//     // Add node to map
//     lookup[n->key] = n; 
//     if(head==nullptr && tail==nullptr) {
//         // first node
//         head = n;
//         tail = n;
//         n->right = nullptr;
//         n->left = nullptr;
//     } else {
//         // update head
//         Node* oldHead = head;
//         oldHead->right = n;
//         n->left = oldHead;
//         n->right = nullptr;
//         head = n;
//     }
// }
// void removeNode(Node* n) {
//     if(!n) {return;}
//     if(n->left) {
//         // update left
//         n->left->right = n->right;
//     } else {
//         // update tail
//         tail = n->right;
//     }if(n->right) {
//         // update right
//         n->right->left = n->left;
//     } else {
//         // update head
//         head = n->left;
//     }
//     n->right = nullptr;
//     n->left = nullptr;
// }
// void moveToFront(Node* n) {
//     if(n != head) {
//         removeNode(n);
//         addToFront(n);
//     }
// }
// void evict() {
//     if(!tail) {return;}
//     if(lookup.size() >= capacity) {
//         Node* n = tail;
//         removeNode(n);
//         lookup.erase(n->key);
//         delete n;
//     }
// }

// public:
//     LRUCache(int capacity) { 
//         this->capacity = capacity;
//         this->head = nullptr;
//         this->tail = nullptr;
//     }
    
//     int get(int key) {
//         if(lookup.count(key)) {
//             Node* n = lookup[key];
//             moveToFront(n);
//             return n->value;
//         } else {
//             return -1;
//         }
//     }
    
//     void put(int key, int value) {
//         if(lookup.count(key)) {
//             Node* n = lookup[key];
//             n->value = value;
//             moveToFront(n);
//         } else {
//             evict();
//             Node* n = new Node(key, value);
//             addToFront(n);
//         }
//     }
// };