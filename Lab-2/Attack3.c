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
    int fake_user = 4; 
    int n = 9;

    printf("Impersonation attack for User_ID = %d\n", fake_user);

    for (int k = 1; k <= 10; k++) {
        int token = modexp(fake_user, k, n);
        printf("k = %d -> Token = %d\n", k, token);

        if (token == 1) {
            printf("\nUse Token = %d to login as User_ID %d\n",
                   token, fake_user);
            break;
        }
    }
    return 0;
}
