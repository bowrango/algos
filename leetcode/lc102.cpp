/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
// class Solution {
// public:
//     vector<vector<int>> levelOrder(TreeNode* root) {
//         // Use BFS with (TreeNode*, depth) and push data into treeData[depth]
//         if(!root) {
//             return {};
//         }

//         vector<vector<int>> treeData;
//         queue<pair<TreeNode*, int>> q;
//         q.push({root, 0});
//         while(!q.empty()) {
//             auto [n, d] = q.front();
//             q.pop();
            
//             // Process this node
//             if(treeData.size() <= d) {
//                 // First node at depth d
//                 treeData.push_back({n->val});
//             } else {
//                 // Add to existing depth
//                 treeData[d].push_back(n->val);
//             }

//             // Process children nodes
//             if(n->left) {
//                 q.push({n->left, d+1});
//             }
//             if(n->right) {
//                 q.push({n->right, d+1});
//             }
//         }
//         return treeData;
//     }
// };
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
    // Use BFS with just TreeNode* since queue processes one level at a time
    if(!root) {
        return {};
    }

    vector<vector<int>> treeData;
    queue<TreeNode*> q;
    q.push(root);
    while(!q.empty()) {
        int sz = q.size();
        vector<int> lvl;
        // Process this level
        while(sz--) {
            TreeNode* n = q.front();
            q.pop();
            lvl.push_back(n->val);
            if(n->left) q.push(n->left);
            if(n->right) q.push(n->right);
        }
        treeData.push_back(lvl);
    }
    return treeData;
}
};