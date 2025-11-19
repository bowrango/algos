// You are eating a candy bar that is made of pieces in a single row (e.g., a Toblerone). You can bite off 1, 2, or 3 pieces at a time. How many different ways can you eat a bar that is n pieces long? 

#include <iostream>
#include <array>

using M = std::array<std::array<int, 3>, 3>;

M matmul(const M& A, const M& B) {
    M C = {0};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            int sum = 0;
            for (int k = 0; k < 3; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    return C;
}

M expm(M base, int exp) {
    // matrix exponentiation by squaring
    // exp = b0 + 2b1 + 4b2 + 8b3 + ...
    M result = {{
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }};
    while (exp > 0) {
        if (exp & 1) {
            // the least significant bit of exp is 1 so 
            // this power contributes to the final result
            result = matmul(result, base);
        }
        // square base and shift exp to examine the next bit
        base = matmul(base, base);
        exp >>= 1;
    }
    return result;
}

int f_best(int n) {

    // Recurrence:
    // f(n) = f(n-1) + f(n-2) + f(n-3)
    // State-Space:
    // [f(n)]    = [1 1 1] [f(n-1)] = M^(n-3)*[f(3)]
    // [f(n-1)]    [1 0 0] [f(n-2)]           [f(2)]   
    // [f(n-2)]    [0 1 0] [f(n-3)]           [f(1)]

    // base cases
    int f1 = 1; // f(1)
    int f2 = 2; // f(2)
    int f3 = 4; // f(3)

    if (n == 0) {return 0;}
    if (n == 1) {return f1;}
    if (n == 2) {return f2;}
    if (n == 3) {return f3;}

    M A = {{
        {1, 1, 1},
        {1, 0, 0},
        {0, 1, 0}
    }};

    // Time: O(logn), Space: O(1)
    M An = expm(A, n-3);

    int fn = An[0][0]*f3 + An[0][1]*f2 + An[0][2]*f1;
    return fn;
}

int f_mine(int n) {
    // f(n) = f(n-1) + f(n-2) + f(n-3)

    // base cases
    int f1 = 1; // f(1)
    int f2 = 2; // f(2)
    int f3 = 4; // f(3)

    if(n==0) {return 0;}
    if(n==1) {return f1;}
    if(n==2) {return f2;}
    if(n==3) {return f3;}

    // we just need a window over the past 3 values
    for (int i=4; i<=n; ++i) {
        // f(i) = f(i-3) + f(i-2) + f(i-1) 
        int fn = f1 + f2 + f3;
        f1 = f2;
        f2 = f3;
        f3 = fn;
    }
    return f3;
}


int main() {

    std::cout << f_mine(4) << std::endl;
    std::cout << candybar(4) << std::endl;
    return 0;
}

