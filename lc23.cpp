
#include <vector>
#include <queue>

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    // struct Compare {
    //     bool operator()(ListNode* a, ListNode* b) {
    //         return a->val > b->val;
    //     }
    // };
    ListNode* mergeTwoLists(ListNode* a, ListNode* b) {
        if(!a) {return b;}
        if(!b) {return a;}

        ListNode dummy(0);
        ListNode* curr = &dummy;

        while (a && b) {
            if(a->val <= b->val) {
                curr->next = a;
                a = a->next;
            } else {
                curr->next = b;
                b = b->next;
            }
            curr = curr->next;
        }

        if(a) {
            curr->next = a;
        } else if (b) {
            curr->next = b;
        }
        return dummy.next;
    }
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        // std::priority_queue<ListNode*, vector<ListNode*>, Compare> q;
        // for(ListNode* l : lists) {
        //     if(l) {
        //         q.push(l);
        //     }
        // }
        // ListNode start(0);
        // ListNode* curr = &start;
        // while(!q.empty()) {
        //     ListNode* n = q.top();
        //     q.pop();
        //     if(n->next) {
        //         q.push(n->next);
        //     }
        //     curr->next = n;
        //     curr = curr->next;
        // }
        // return start.next;

        if(lists.empty()) {return nullptr;}
        size_t k = lists.size();
        size_t win = 1;
        // merge in pairs of win size
        while(win < k) {
            // even indices store the merged list
            for(size_t i = 0; i < k - win; i+=2*win) {
                lists[i] = mergeTwoLists(lists[i], lists[i+win]);
            }
            win *= 2;
        }
        return lists[0];
    }
};