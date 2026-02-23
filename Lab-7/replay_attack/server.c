#include <stdio.h>
#include <winsock2.h>
#include <math.h>

#pragma comment(lib,"ws2_32.lib")

/* Fast modular exponentiation */
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
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientLen = sizeof(clientAddr);

    long long p, g;
    long long Xb;   // Server private key
    long long Yb;   // Server public key
    long long Ya;   // Client / Replay attacker public key
    long long sharedKey;

    WSAStartup(MAKEWORD(2,2), &wsa);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    printf("Waiting for client connection...\n");
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

    printf("\n--- SERVER SIDE (REPLAY VULNERABLE) ---\n");

    /* Input and validate prime p */
    do {
        printf("Enter Prime Number (p): ");
        scanf("%lld", &p);
        if (!isPrime(p))
            printf("Error: p is not prime. Enter again.\n");
    } while (!isPrime(p));

    /* Input and validate generator g */
    do {
        printf("Enter Generator (g): ");
        scanf("%lld", &g);
        if (!isPrimitiveRoot(g, p))
            printf("Error: g is not a primitive root of p. Enter again.\n");
    } while (!isPrimitiveRoot(g, p));

    /* Server private key */
    printf("Enter Server Private Key (Xb): ");
    scanf("%lld", &Xb);

    /* Compute server public key */
    Yb = modexp(g, Xb, p);
    printf("Computed Server Public Key:\n");
    printf("Yb = g^Xb mod p = %lld\n", Yb);

    /* Verify g^(p-1) mod p */
    printf("Verification: g^(p-1) mod p = %lld\n", modexp(g, p - 1, p));

    /* Send public values */
    send(clientSocket, (char*)&p, sizeof(p), 0);
    send(clientSocket, (char*)&g, sizeof(g), 0);
    send(clientSocket, (char*)&Yb, sizeof(Yb), 0);

    printf("Sent (p, g, Yb) to client\n");

    /* Receive Ya (from real client OR replay attacker) */
    recv(clientSocket, (char*)&Ya, sizeof(Ya), 0);
    printf("Received Client Public Key Ya = %lld\n", Ya);

    /* Compute shared secret */
    sharedKey = modexp(Ya, Xb, p);
    printf("Computed Shared Secret Key:\n");
    printf("K = Ya^Xb mod p = %lld\n", sharedKey);

    printf("\n[WARNING] Server does NOT verify freshness of Ya.\n");
    printf("[REPLAY ATTACK POSSIBLE]\n");

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}