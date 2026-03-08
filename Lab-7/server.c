#include <stdio.h>
#include <winsock2.h>
#include <math.h>

#pragma comment(lib,"ws2_32.lib")

long long modexp(long long b, long long e, long long m) {
    long long r = 1;
    b %= m;
    while (e > 0) {
        if (e & 1) r = (r * b) % m;
        b = (b * b) % m;
        e >>= 1;
    }
    return r;
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

/* Find prime factors of n */
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
    SOCKET sfd, cfd;
    struct sockaddr_in server, client;
    int c = sizeof(client);

    long long p, g;
    long long Xb;   // Server private key
    long long Yb;   // Server public key
    long long Ya;   // Client public key
    long long key;

    WSAStartup(MAKEWORD(2,2), &wsa);
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sfd, (struct sockaddr*)&server, sizeof(server));
    listen(sfd, 1);

    printf("Waiting for client...\n");
    cfd = accept(sfd, (struct sockaddr*)&client, &c);

    printf("\n--- SERVER SIDE ---\n");

    do {
        printf("Enter Prime Number (p): ");
        scanf("%lld", &p);
        if (!isPrime(p))
            printf("Error: p is not prime. Enter again.\n");
    } while (!isPrime(p));

    do {
        printf("Enter Generator (g): ");
        scanf("%lld", &g);
        if (!isPrimitiveRoot(g, p))
            printf("Error: g is not a primitive root of p. Enter again.\n");
    } while (!isPrimitiveRoot(g, p));

    printf("Enter Server Private Key (Xb): ");
    scanf("%lld", &Xb);

    while(Xb>=p){
        printf("Enter Client Private Key (Xb) < p-1: ");
        scanf("%lld", &Xb);
    }

    Yb = modexp(g, Xb, p);
    printf("Computed Server Public Key (Yb = g^Xb mod p): %lld\n", Yb);

    /* Send public values */
    send(cfd, (char*)&p, sizeof(p), 0);
    send(cfd, (char*)&g, sizeof(g), 0);
    send(cfd, (char*)&Yb, sizeof(Yb), 0);

    /* Receive client public key */
    recv(cfd, (char*)&Ya, sizeof(Ya), 0);
    printf("Received Client Public Key (Ya): %lld\n", Ya);

    /* Compute shared key */
    key = modexp(Ya, Xb, p);
    printf("Server Shared Secret Key = (Ya^Xb) mod p = %lld\n", key);

    closesocket(cfd);
    closesocket(sfd);
    WSACleanup();
    return 0;
}