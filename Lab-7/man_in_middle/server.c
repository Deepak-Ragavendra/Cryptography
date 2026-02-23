#include <stdio.h>
#include <winsock2.h>
#include <math.h>

#pragma comment(lib,"ws2_32.lib")

/* Fast modular exponentiation */
long long modexp(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1)
            res = (res * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return res;
}

/* Check if p is prime */
int isPrime(long long n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

/* Get prime factors of n */
int getPrimeFactors(long long n, long long factors[]) {
    int count = 0;
    for (long long i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            factors[count++] = i;
            while (n % i == 0)
                n /= i;
        }
    }
    if (n > 1)
        factors[count++] = n;
    return count;
}

/* Check if g is a primitive root of p */
int isPrimitiveRoot(long long g, long long p) {
    long long phi = p - 1;
    long long factors[64];
    int fcount = getPrimeFactors(phi, factors);

    for (int i = 0; i < fcount; i++) {
        if (modexp(g, phi / factors[i], p) == 1)
            return 0;
    }
    return 1;
}

int main() {
    WSADATA wsa;
    SOCKET serverSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    long long p, g;
    long long Xb;   // Server private key
    long long Yb;   // Server public key
    long long Ya;   // Client public key
    long long sharedKey;

    WSAStartup(MAKEWORD(2,2), &wsa);

    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9090);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSock, 1);

    printf("Waiting for client...\n");
    clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &addrLen);

    printf("\n--- SERVER SIDE ---\n");

    /* Validate prime p */
    do {
        printf("Enter prime number (p): ");
        scanf("%lld", &p);
        if (!isPrime(p))
            printf("Error: p is not prime. Enter again.\n");
    } while (!isPrime(p));

    /* Validate generator g */
    do {
        printf("Enter generator (g): ");
        scanf("%lld", &g);
        if (!isPrimitiveRoot(g, p))
            printf("Error: g is not a primitive root of p. Enter again.\n");
    } while (!isPrimitiveRoot(g, p));

    printf("Enter Server Private Key (Xb): ");
    scanf("%lld", &Xb);

    /* Compute server public key */
    Yb = modexp(g, Xb, p);
    printf("Computed Server Public Key Yb = g^Xb mod p = %lld\n", Yb);

    /* Verify g^(p-1) mod p */
    printf("Verification: g^(p-1) mod p = %lld\n", modexp(g, p - 1, p));

    /* Send public values */
    send(clientSock, (char*)&p, sizeof(p), 0);
    send(clientSock, (char*)&g, sizeof(g), 0);
    send(clientSock, (char*)&Yb, sizeof(Yb), 0);

    /* Receive client public key */
    recv(clientSock, (char*)&Ya, sizeof(Ya), 0);
    printf("Received Client Public Key Ya = %lld\n", Ya);

    /* Compute shared secret */
    sharedKey = modexp(Ya, Xb, p);
    printf("Server Shared Secret Key = (Ya^Xb) mod p = %lld\n", sharedKey);

    closesocket(clientSock);
    closesocket(serverSock);
    WSACleanup();
    return 0;
}