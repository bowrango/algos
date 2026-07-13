class Solution {
private:
    string expression;
    vector<vector<vector<int>>> memo;
    vector<vector<bool>> computed;

    const vector<int>& evaluate(int left, int right) {
        if(computed[left][right]) {
            // Return previous results
            return memo[left][right];
        }

        // Get previous results
        vector<int>& results = memo[left][right];

        // Find midpoint operator at index i,
        // recurse and left and right subexpressions
        // and combine results
        bool foundOp = false;
        for(int i = left; i < right; ++i) {
            char op = expression[i];
            if(op != '+' && op != '-' && op != '*') {
                continue;
            }
            foundOp = true;

            // Evaluate children
            const vector<int>& leftResult = evaluate(left, i-1);
            const vector<int>& rightResult = evaluate(i+1, right);

            // Evaluate op for all combinations of left and right results
            for(int leftValue : leftResult) {
                for(int rightValue : rightResult) {
                    if(op == '+') {
                        results.push_back(leftValue+rightValue);
                    } 
                    else if(op == '-') {
                        results.push_back(leftValue-rightValue);
                    }
                    else {
                        results.push_back(leftValue*rightValue);
                    }
                }
            }
        }

        if(!foundOp) {
            // expression[left...right] is just int
            results.push_back(stoi(expression.substr(left, right-left+1)));
        }

        computed[left][right] = true;
        return results;
    };
public:
    vector<int> diffWaysToCompute(string input) {
        // Use memoization to store results from each substring
        // memo[left][right] = all results from expression[left...right]
        // Track evaluated expressions
        // computed[left][right] = true when already evaluated
        expression = std::move(input);
        int n = expression.size();
        memo.resize(n, vector<vector<int>>(n));
        computed.resize(n, vector<bool>(n, false));
        return evaluate(0, n-1);
    }
};