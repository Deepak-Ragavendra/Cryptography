#include <stdio.h>

long long modexp(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}


int main() {
    int user_id = 2;  
    int n = 9;       
    int token_seen = 2;

    printf("Predicting valid tokens for User_ID = %d\n", user_id);

    for (int k = 1; k <= 20; k++) {
        int token = modexp(user_id, k, n);
        if (token == token_seen) {
            printf("k = %d produces Token = %d  <-- VALID\n", k, token);
        }
    }
    return 0;
}
