#include <stdio.h>
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

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serverAddr;

    long long p, g;
    long long Xa;   // Client private key
    long long Ya;   // Client public key
    long long Yb;   // Server public key
    long long sharedKey;

    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);   // Connects to attacker / server
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    /* Receive public parameters from server */
    recv(sock, (char*)&p, sizeof(p), 0);
    recv(sock, (char*)&g, sizeof(g), 0);
    recv(sock, (char*)&Yb, sizeof(Yb), 0);

    printf("\n--- CLIENT SIDE ---\n");
    printf("Prime (p) = %lld\n", p);
    printf("Generator (g) = %lld\n", g);
    printf("Received Server Public Key Yb = %lld\n", Yb);

    /* Input client private key */
    printf("Enter Client Private Key (Xa): ");
    scanf("%lld", &Xa);

    /* Compute client public key */
    Ya = modexp(g, Xa, p);
    printf("Computed Client Public Key Ya = g^Xa mod p = %lld\n", Ya);

    /* Send client public key */
    send(sock, (char*)&Ya, sizeof(Ya), 0);

    /* Compute shared secret */
    sharedKey = modexp(Yb, Xa, p);
    printf("Client Shared Secret Key = (Yb^Xa) mod p = %lld\n", sharedKey);

    closesocket(sock);
    WSACleanup();
    return 0;
}