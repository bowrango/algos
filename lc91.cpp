class Solution {
public:
    int numDecodings(string s) {

        int n = s.size();
        if(n==0) {return 0;}
        if(s[0]=='0') {
            // impossible code
            return 0;
        }

        // ways[i] = number of ways to decode s[0...i-1]
        // store the 2 previous iterates because codesize is max 2
        int prev2 = 1; // ways[i-2]=ways[0] (empty)
        int prev1 = 1;  // ways[i-1]=ways[1] (valid 1st digit)

        // accumulate number of ways for each code digit 
        int curr; // ways[i]
        int one, two; // the 1 and 2 digit codes
        for(size_t i = 2; i <= n; i++) {
            curr = 0;
            one = s[i-1] - '0';
            if(one >= 1 && one <= 9) {
                // add valid 1-digit
                curr += prev1;
            }
            two = 10*(s[i-2]-'0') + one;
            if(two >= 10 && two <= 26) {
                // add valid 2-digit
                curr += prev2;
            }
            prev2 = prev1;
            prev1 = curr;
        } 
        return prev1; // ways[n]
    }
};