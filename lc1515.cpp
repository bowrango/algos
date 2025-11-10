class Solution {
public:
    double getMinDistSum(vector<vector<int>>&positions) {
    /* ---------- 1. start at the centroid ---------- */
    const int n = positions.size();
    vector<pair<double, double>> pts;
    pts.reserve(n);
    for (auto& p: positions) {pts.emplace_back(p[0], p[1]);};
    double cx = 0.0;
    double cy = 0.0;
    for (auto [x,y]: pts) {
        cx += x;
        cy += y;
    }
    cx /= n;
    cy /= n;

    auto cost = [&](double x, double y) {
        double s = 0.0;
        for (auto [px,py] : pts) {
            s += hypot(px-x, py-y);
        }
        return s;
    };
    /* ---------- 2. initial step size = farthest point from centroid ---------- */
    double best = cost(cx, cy);
    double step = 0.0;
    for (auto [px,py]: pts) {
        step = max(step, hypot(px-cx, py-cy));
    }
    /* ---------- 3. shrink-and-search loop ---------- */
    const double esp = 1e-6;
    static const int dirs[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};
    while (step > esp) {
        bool improved = false;
        for (auto d: dirs) {
            double nx = cx + d[0]*step;
            double ny = cy + d[1]*step;
            double c = cost(nx, ny);
            if (c < best) {
                best = c;
                cx = nx;
                cy = ny;
                improved = true;
                break;
            }
        }
        if (!improved) {step /= 2;}   
    }
    return best;
}
};

