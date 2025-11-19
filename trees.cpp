
// g++ trees.cpp -o trees -std=c++17

#include <iostream>

template <typename T>
class Node {
    public:
    T data;
    Node* left;
    Node* right;
    Node(T value) :data(value), left(nullptr), right(nullptr) {};
};

template<typename T>
class SplayTree {
    public:
    SplayTree() : root(nullptr) {}
    ~SplayTree() {clear(root);}

    private:
    Node<T>* root;

    Node<T>* rightRotate(Node<T>* x) {
        Node<T>* y = x->left;
        x->left = y->right;
        y->right = x;
    }
    Node<T>* leftRotate(Node<T>* x) {
        Node<T>* y = x->right;
        x->right = y->left;
        y->left = x;
    }

};


template <typename T>
void invert(Node<T>* root) {
    if (root == nullptr) {return;}

    invert(root->left);
    invert(root->right);

    std::swap(root->left, root->right);
}

template <typename T>
void print(Node<T>* root) {
    if (!root) {return;}
    std::cout << root->data << " ";
    print(root->left);
    print(root->right);
}

template <typename T>
void cleanup(Node<T>* root) {
    if (!root) {return;}
    cleanup(root->left);
    cleanup(root->right);
    delete root;
}

int main() {

    Node<int>* root = new Node<int>(1);
    root->left = new Node<int>(2);
    root->right = new Node<int>(3);

    root->right->left = new Node<int>(4);
    root->right->right = new Node<int>(5);

    print(root); // 1 2 3 4 5

    invert(root);

    print(root); // 1 3 5 4 2

}