class Solution {
public:
    void lookLeft(queue<pair<int,int>>& q, 
                  const vector<vector<int>>& grid,
                  const int y, const int x, const int n, const int m) {
        // Valid streets to the left are 1,4,6
        if(x-1 >= 0 && (grid[y][x-1]==1 || grid[y][x-1]==4 || grid[y][x-1]==6)) {
            q.push({y,x-1});
        }
    }
    void lookRight(queue<pair<int,int>>& q, 
                const vector<vector<int>>& grid,
                const int y, const int x, const int n, const int m) {
        // Valid streets to the rigth are 1,3,5
        if(x+1 < m && (grid[y][x+1]==1 || grid[y][x+1]==3 || grid[y][x+1]==5)) {
            q.push({y,x+1});
        }
    }
    void lookUp(queue<pair<int,int>>& q, 
            const vector<vector<int>>& grid,
            const int y, const int x, const int n, const int m) {
        // Valid streets to the top are 2,3,4
        if(y-1 >= 0 && (grid[y-1][x]==2 || grid[y-1][x]==3 || grid[y-1][x]==4)) {
            q.push({y-1,x});
        }
    }
    void lookDown(queue<pair<int,int>>& q, 
        const vector<vector<int>>& grid,
        const int y, const int x, const int n, const int m) {
        // Valid streets to the bottom are 2,5,6
        if(y+1 < n && (grid[y+1][x]==2 || grid[y+1][x]==5 || grid[y+1][x]==6)) {
            q.push({y+1,x});
        }
    }
    bool hasValidPath(vector<vector<int>>& grid) {
        // Use breadth-first search over valid cells and keep track of
        // visited cells
        int n = grid.size(); // num rows
        if(n==0) {return false;}
        int m = grid[0].size(); // num cols
        queue<pair<int,int>> q;
        vector<vector<bool>> seen(n, vector<bool>(m,false));
        // current position
        int y = 0;
        int x = 0;
        q.push({y,x});
        while(!q.empty()) {
            // update position
            auto [y,x] = q.front();
            q.pop();
            if(seen[y][x]) {
                // avoid backtracking
                continue;
            } else {
                // mark visited cell
                seen[y][x] = true;
            }
            if(x==m-1 && y==n-1) {return true;}
            // determine next valid cells
            int s = grid[y][x];
            if(s==1) {
                lookLeft(q,grid,y,x,n,m);
                lookRight(q,grid,y,x,n,m);
            }
            if(s==2) {
                lookUp(q,grid,y,x,n,m);
                lookDown(q,grid,y,x,n,m);
            }
            if(s==3) {
                lookLeft(q,grid,y,x,n,m);
                lookDown(q,grid,y,x,n,m);
            }
            if(s==4) {
                lookRight(q,grid,y,x,n,m);
                lookDown(q,grid,y,x,n,m);
            }
            if(s==5) {
                lookLeft(q,grid,y,x,n,m);
                lookUp(q,grid,y,x,n,m);
            }
            if(s==6) {
                lookUp(q,grid,y,x,n,m);
                lookRight(q,grid,y,x,n,m);
            }
        }
        if(x==m-1 && y==n-1) {return true;}
        else {return false;}
    }
};