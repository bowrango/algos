class Solution {
public:
    // int findKthLargest(vector<int>& nums, int k) {
    //     // Use min-heap with size k to maintain the largest k nums.
    //     // O(n(log(k)+log(k))) = O(nlogk) runtime
    //     priority_queue<int, vector<int>, greater<int>> pq;
    //     for(auto& x : nums) {
    //         pq.push(x);
    //         if(pq.size() > k) {
    //             // Remove smallest
    //             pq.pop();}
    //     }
    //     return pq.top();
    // }
    int quickselect(vector<int>& nums, int lo, int hi, int trgt) {
        while(lo <= hi) {
            // Random pivot in [lo hi]
            int pivotIdx = lo + rand()%(hi-lo+1);
            int pivot = nums[pivotIdx];
            // Partition
            int left = lo;
            int right = hi;
            int i = lo;
            while(i <= right) {
                if(nums[i] < pivot) {
                    swap(nums[i], nums[left]);
                    i++;
                    left++;
                } else if (nums[i] > pivot) {
                    swap(nums[i], nums[right]);
                    right--;
                } else {
                    i++;
                }
            }
            if(trgt >= left && trgt <= right) {
                return nums[trgt];
            }
            if(trgt < left) {
                // Search left
                hi = left-1;
            } else {
                // Search right
                lo = right+1;
            }
        }
        return -1;
    }
    int findKthLargest(vector<int>& nums, int k) {
        return quickselect(nums, 0, nums.size()-1, nums.size()-k);
    }
};