#include <cstdio>
#include <iostream>
#include <algorithm>
#include <string>
#include <cassert>

// Given a parenthesized string representing a single-rooted tree,
// return the max nesting depth. Returns -1 if the string is invalid.
int treeDepth(const std::string& s) {
    if (s.empty()) return -1;
    if (s.front() != '(' || s.back() != ')') return -1;

    int lvl = 0;
    int depth = 0;
    int n = s.size();
    for (int i = 0; i < n; ++i) {
        char c = s[i];
        if (c == '(') {
            ++lvl;
            depth = std::max(depth, lvl);
        } else if (c == ')') {
            --lvl;
            if (lvl < 0) return -1;
            if (lvl == 0 && i != n - 1) return -1;
        } else {
            return -1;
        }
    }
    return (lvl == 0) ? depth : -1;
}

int main() {
    // Single node tree: ()
    assert(treeDepth("()") == 1);

    // Two levels: (())
    assert(treeDepth("(())") == 2);

    // Three levels deep: ((()))
    assert(treeDepth("((()))") == 3);

    // Root with two children: (()())
    assert(treeDepth("(()())") == 2);

    // Deeper mixed tree: ((())(()))
    assert(treeDepth("((())(()))") == 3);

    // Invalid: unmatched open
    assert(treeDepth("(()") == -1);

    // Invalid: two separate roots
    assert(treeDepth("()()") == -1);

    // Invalid: empty string
    assert(treeDepth("") == -1);

    // Invalid: bad characters
    assert(treeDepth("(a)") == -1);

    // Invalid: starts wrong
    assert(treeDepth(")(") == -1);

    return 0;
}
