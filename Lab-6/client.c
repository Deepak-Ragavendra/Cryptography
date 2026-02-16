#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

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

int isPrime(long long n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long long i = 3; i * i <= n; i += 2)
        if (n % i == 0)
            return 0;
    return 1;
}

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    long long p, q, e, N, phi;
    char message[100];
    long long encrypted[100];
    int len;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");  // localhost for testing

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }

    printf("Connected to server\n\n");

    printf("Enter p (prime): ");
    scanf("%lld", &p);
    printf("Enter q (prime): ");
    scanf("%lld", &q);

    if (!isPrime(p) || !isPrime(q)) {
        printf("Error: p and q must be prime\n");
        closesocket(s);
        WSACleanup();
        return 1;
    }

    if (p == q) {
        printf("Error: p and q should be different\n");
        closesocket(s);
        WSACleanup();
        return 1;
    }

    N = p * q;
    phi = (p - 1) * (q - 1);

    printf("Enter e (public exponent): ");
    scanf("%lld", &e);

    if (e <= 1 || e >= phi || gcd(e, phi) != 1) {
        printf("Error: e must be coprime with phi(N) and 1 < e < phi\n");
        closesocket(s);
        WSACleanup();
        return 1;
    }

    printf("Enter message (max 99 chars): ");
    scanf(" %99s", message);

    len = strlen(message);

    printf("\nPublic Key (N, e): (%lld, %lld)\n", N, e);
    printf("Encrypting message...\n\n");

    for (int i = 0; i < len; i++) {
        encrypted[i] = modexp((int)message[i], e, N);
        printf("Encrypted '%c' (ASCII %d) as %lld\n", message[i], (int)message[i], encrypted[i]);
    }

    // Send p, q, e, len, and encrypted data
    if (send(s, (char*)&p, sizeof(p), 0) == SOCKET_ERROR) {
        printf("Send failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }
    send(s, (char*)&q, sizeof(q), 0);
    send(s, (char*)&e, sizeof(e), 0);
    send(s, (char*)&len, sizeof(len), 0);
    send(s, (char*)encrypted, sizeof(long long) * len, 0);

    printf("\nEncrypted data sent to server\n");

    closesocket(s);
    WSACleanup();
    return 0;
}