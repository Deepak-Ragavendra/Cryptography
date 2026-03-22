#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib,"Ws2_32.lib")

long long modexp(long long a, long long b, long long m) {
    long long res = 1;
    a = a % m;
    while (b > 0) {
        if (b % 2 == 1)
            res = (res * a) % m;
        a = (a * a) % m;
        b = b / 2;
    }
    return res;
}

long long gcd(long long a, long long b) {
    while (b != 0) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int isPrime(long long n) {
    long long i;
    if (n < 2) return 0;
    for (i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

long long modInverse(long long a, long long m) {
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m;
        a = t;

        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
        x1 += m0;

    return x1;
}

long long hashMessage(char msg[], long long q) {
    long long h = 0;
    int i;
    for (i = 0; msg[i] != '\0'; i++) {
        h = (h + msg[i]) % q;
    }
    return h;
}

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    long long p, q, h, g, x, y, k, r, sig, hm;
    int L;
    char msg[256];
    char buffer[1024];

    srand(time(NULL));

    while (1) {
        printf("Enter L: ");
        scanf("%d", &L);

        printf("Enter prime p such that 2^(L-1) < p < 2^L : ");
        scanf("%lld", &p);

        if (!isPrime(p)) {
            printf("p is not prime. Enter again.\n");
            continue;
        }

        if (!(p > (1LL << (L - 1)) && p < (1LL << L))) {
            printf("p not in range. Enter again.\n");
            continue;
        }

        break;
    }

    while (1) {
        printf("Enter prime q such that q divides (p-1): ");
        scanf("%lld", &q);

        if (!isPrime(q)) {
            printf("q is not prime. Enter again.\n");
            continue;
        }

        if ((p - 1) % q != 0) {
            printf("q does not divide (p-1). Enter again.\n");
            continue;
        }

        break;
    }

    while (1) {
        printf("Enter h such that 1 < h < p-1: ");
        scanf("%lld", &h);

        if (h <= 1 || h >= p - 1) {
            printf("Invalid h. Enter again.\n");
            continue;
        }

        g = modexp(h, (p - 1) / q, p);

        if (g <= 1) {
            printf("Computed g is invalid. Enter another h.\n");
            continue;
        }

        break;
    }

    x = (rand() % (q - 1)) + 1;   // 0 < x < q
    y = modexp(g, x, p);

    getchar();
    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    msg[strcspn(msg, "\n")] = '\0';

    hm = hashMessage(msg, q);

    while (1) {
        k = (rand() % (q - 1)) + 1;   // 0 < k < q
        if (gcd(k, q) != 1) continue;

        r = modexp(g, k, p) % q;
        if (r == 0) continue;

        sig = (modInverse(k, q) * (hm + x * r)) % q;
        if (sig == 0) continue;

        break;
    }

    printf("\nClient Side:\n");
    printf("g = %lld\n", g);
    printf("Private key x = %lld\n", x);
    printf("Public key y  = %lld\n", y);
    printf("k = %lld\n", k);
    printf("r = %lld\n", r);
    printf("s = %lld\n", sig);
    printf("Sign = (%lld, %lld)\n", r, sig);

    WSAStartup(MAKEWORD(2,2), &wsa);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8080);

    connect(s, (struct sockaddr *)&server, sizeof(server));

    sprintf(buffer, "%lld\n%lld\n%lld\n%lld\n%lld\n%lld\n%s",
            p, q, g, y, r, sig, msg);

    send(s, buffer, strlen(buffer) + 1, 0);

    closesocket(s);
    WSACleanup();

    return 0;
}