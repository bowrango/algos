// self-balancing binary search tree

// g++ bbt.cpp -o bbt -std=c++17

// insertion O(logn)
// deletion O(logn)

#include <cstddef>
#include <algorithm>
#include <iostream>
#include <string>

template<typename T>
class BBT {
    private:
    struct node {
        T value;
        node* left;
        node* right;
        // height of the subtree rooted at this node
        std::size_t h;
        node(T v) : value(v),left(nullptr),right(nullptr),h(1) {}
    };
    node* root{nullptr};

    int height(node* n) {
        if(!n) {return 0;}
        return n->h;
    }

    void updateHeight(node* n) {
        n->h = 1 + std::max(height(n->left), height(n->right));
    }

    int balance(node* n) {
        return height(n->left) - height(n->right);
    }

    node* rotateRight(node* n) {
        node* newRoot = n->left;
        node* moved = newRoot->right;
        n->left = moved;
        newRoot->right = n;
        updateHeight(n);
        updateHeight(newRoot);
        return newRoot;
    }

    node* rotateLeft(node* n) {
        node* newRoot = n->right;
        node* moved = newRoot->left;
        n->right = moved;
        newRoot->left = n;
        updateHeight(n);
        updateHeight(newRoot);
        return newRoot;
    }

    node* insert_(node* current, T value) {
        // return what the subtree root should be
        if(!current) {
            node* n = new node(value);
            return n;
        }
        // BST Insertion
        if(value < current->value) {
            current->left = insert_(current->left, value);
        } 
        else if(value > current->value) {
            current->right = insert_(current->right, value);
        } else {
            return current;
        }
        // Self-Balance
        updateHeight(current);
        int bal = balance(current);
        if(bal > 1) { // left-heavy
            if(value < current->left->value) { // left-left
                return rotateRight(current);
            } else { // left-right
                current->left = rotateLeft(current->left);
                return rotateRight(current);

            }
        } else if(bal < -1) { // right-heavy
            if(value > current->right->value) { // right-right
                return rotateLeft(current);
            } else {  // right-left
                current->right = rotateRight(current->right);
                return rotateLeft(current);
            }
        } else {
            // no rebalance
            return current;
        }
    }

    void printTree_(node* current, const std::string& prefix, bool isLast, const std::string& label) {
        if(!current) {
            return;
        }

        std::cout << prefix;
        if(!label.empty()) {
            std::cout << (isLast ? "`-- " : "|-- ") << label;
        }

        std::cout << current->value << " (h=" << current->h << ")\n";

        if(current->left) {
            printTree_(current->left,
                       prefix + (label.empty() ? "" : (isLast ? "    " : "|   ")),
                       !current->right,
                       "L: ");
        }

        if(current->right) {
            printTree_(current->right,
                       prefix + (label.empty() ? "" : (isLast ? "    " : "|   ")),
                       true,
                       "R: ");
        }
    }

    public:
    void insert(T value) {
        root = insert_(root, value);
    }

    void remove(T value) {}

    void printTree() {
        if(!root) {
            std::cout << "(empty)\n";
            return;
        }
        printTree_(root, "", true, "");
    }

};


int main() {

    BBT<int> t;
    t.insert(0);
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.insert(4);
    t.insert(5);
    t.printTree();

    return 0;
}
