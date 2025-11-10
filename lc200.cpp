class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        // Find number of connected components
        // Use BFS with queue
        const int m = grid.size();
        const int n = grid[0].size();
        vector<vector<bool>> seen(m, vector<bool>(n,0));
        const int dirs[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};

        auto inside = [&](int r, int c) {
            return 0<=r && r<m && 0<=c && c<n;
        };
        // Run BFS
        auto bfs = [&](int sr, int sc) {
            queue<pair<int,int>> q;
            q.push({sr,sc});
            seen[sr][sc] = true;
            while (!q.empty()) {
                auto [r,c] = q.front();
                q.pop();
                for(auto [dr,dc] : dirs) {
                    int nr = r+dr;
                    int nc = c+dc;
                    if(!inside(nr,nc) || seen[nr][nc]) {continue;}
                    if(grid[nr][nc]=='0') {continue;} // skip water
                    seen[nr][nc] = true;
                    q.push({nr,nc});
                }
            }
        };
        // Count connected components
        int comps = 0;
        for(int r=0; r<m; r++) {
            for(int c=0; c<n; c++) {
                if(!seen[r][c] && grid[r][c]=='1') {
                    comps++;
                    bfs(r,c);
                }
            }
        }
        return comps;
    }
};