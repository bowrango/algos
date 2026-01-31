# Binary Tree Parser
#
# Parses a list of (parent,child) edge pairs and outputs the tree as an S-expression.
# Input:  "(A,B) (A,C) (B,D)" - space-separated parent-child pairs
# Output: "(A(B(D))(C))" - nested S-expression representing the tree
#
# Errors:
#   E1 - Input format (whitespace, pair syntax)
#   E2 - No duplicate edges
#   E3 - Binary constraint (max 2 children per node)
#   E4 - Exactly one root (node with no parent)
#   E5 - No cycles (edges = nodes - 1 for a tree)

import re
import sys
from collections import defaultdict

# TODO replace slow REGEX
# pair format (A-Z,A-Z)
PAIR = re.compile(r"\(([A-Z]),([A-Z])\)")
# list of pairs format
LINE = re.compile(r"\([A-Z],[A-Z]\)(?: \([A-Z],[A-Z]\))*\Z")

def s_expression(node: set, children: dict) -> str:
    """Recursively build S-expression by DFS traversal."""
    kids = sorted(children.get(node, []))
    return "("+node+"".join(s_expression(c, children) for c in kids)+")"

def parse(line: str):
    if line != line.strip():
        # invalid
        print("E1")
        return None
    if not LINE.fullmatch(line):
        # invalid
        print("E1")
        return None

    pairs = PAIR.findall(line)
    
    children = defaultdict(list)
    parents = {}
    nodes = set()
    seen = set()
    for (p,c) in pairs:
        if (p,c) in seen:
            # duplicate pair
            print("E2")
            return
        seen.add((p,c))
        if len(children[p]) == 2:
            # more than 2 children
            print("E3")
            return
        nodes.update((p,c))
        children[p].append(c)
        parents[c] = p

    root = list(nodes - set(parents))
    if len(root) != 1:
        # no or multiple roots
        print("E4")
        return
    if len(pairs) != len(nodes)-1:
        # cycle in binary tree
        print("E5")
        return

    print(s_expression(root[0], children))
        
def main():
    line = "(A,B) (A,C) (B,D) (D,E) (C,F) (E,G)"
    # line = "awecihjaw;elkifjhasz"
    # line = "(A,B) (A,C) (X,Y) (X,Z)"
    # line = sys.stdin.read()
    # line = "(A,B) (B,C) (C,A)"
    # line = "(A,B) (C,D)"
    parse(line)

    
if __name__ == "__main__":
    main()