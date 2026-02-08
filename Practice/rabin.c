#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* (a^b) % mod using fast exponentiation */
long long modPower(long long a, long long b, long long mod) {
    long long result = 1;
    a %= mod;

    while (b > 0) {
        if (b & 1)
            result = (result * a) % mod;
        a = (a * a) % mod;
        b >>= 1;
    }
    return result;
}

/* Single Miller–Rabin test */
int millerTest(long long d, long long n) {
    long long a = 2 + rand() % (n - 4);
    long long x = modPower(a, d, n);

    if (x == 1 || x == n - 1)
        return 1;

    while (d != n - 1) {
        x = (x * x) % n;
        d *= 2;

        if (x == 1) return 0;
        if (x == n - 1) return 1;
    }

    return 0;
}

/* Main Miller–Rabin function */
int isPrime(long long n, int k) {
    if (n <= 1 || n == 4) return 0;
    if (n <= 3) return 1;

    long long d = n - 1;
    while ((d & 1) == 0)
        d >>= 1;

    for (int i = 0; i < k; i++)
        if (!millerTest(d, n))
            return 0;

    return 1;
}

int main() {
    long long n;
    int k = 5;  // number of iterations

    srand(time(NULL));

    printf("Enter number: ");
    scanf("%lld", &n);

    if (isPrime(n, k))
        printf("Prime\n");
    else
        printf("Composite\n");

    return 0;
}