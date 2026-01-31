
constexpr array<int, 26> EMPTY = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

class Trie {
// Trie or A-Z Tree
private:
// trie[depth][width]
vector<array<int, 26>> trie;
vector<bool> leaf;
public:
    Trie() {
        // 1 (root) + 30,000 (total calls)
        trie.reserve(30001);
        leaf.reserve(30001);
        // initialize root
        trie.push_back(EMPTY);
        leaf.push_back(false);
    }
    
    int find(const string& word) {
        int cur = 0;
        for(char c : word) {
            int idx = c - 'a';
            if(trie[cur][idx] == -1) {return -1;}
            cur = trie[cur][idx];
        }
        return cur;
    }
    
    void insert(const string& word) {
        int cur = 0;
        for(char c : word) {
            int idx = c - 'a';
            if(trie[cur][idx] == -1) {
                trie.push_back(EMPTY);
                leaf.push_back(false);
                trie[cur][idx] = trie.size() - 1;
            }
            cur = trie[cur][idx];
        }
        // last node ends the word
        leaf[cur] = true;
    }
    
    bool search(const string& word) {
        int n = find(word);
        return n != -1 && leaf[n];
    }
    
    bool startsWith(const string& prefix) {
        int n = find(prefix);
        return n != -1;
    }
};