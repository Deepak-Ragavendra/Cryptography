#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

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
    for (long long i = 2; i * i <= n; i++)
        if (n % i == 0)
            return 0;
    return 1;
}

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    long long p, q, e, N;
    char message[100];
    long long encrypted[100];
    int len;

    WSAStartup(MAKEWORD(2,2), &wsa);
    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8888);
    server.sin_addr.s_addr = inet_addr("192.168.1.7");

    connect(s, (struct sockaddr*)&server, sizeof(server));

    printf("Enter p: ");
    scanf("%lld", &p);
    printf("Enter q: ");
    scanf("%lld", &q);

    if (!isPrime(p) || !isPrime(q)) {
        printf("Error: p and q must be prime\n");
        return 0;
    }

    printf("Enter e: ");
    scanf("%lld", &e);

    printf("Enter message: ");
    scanf("%s", message);

    N = p * q;
    len = strlen(message);

    for (int i = 0; i < len; i++) {
        encrypted[i] = modexp((int)message[i], e, N);
        printf("Encrypted '%c' as %lld\n", message[i], encrypted[i]);
    }

    send(s, (char*)&p, sizeof(p), 0);
    send(s, (char*)&q, sizeof(q), 0);
    send(s, (char*)&e, sizeof(e), 0);
    send(s, (char*)&len, sizeof(len), 0);
    send(s, (char*)encrypted, sizeof(long long) * len, 0);

    printf("Encrypted data sent to server\n");

    closesocket(s);
    WSACleanup();
    return 0;
}