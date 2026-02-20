// dh_client.c
#include <stdio.h>
#include <winsock2.h>

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

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    long long p, g;
    long long Xb;   // Client private key
    long long Ya;   // Server public key
    long long Yb;   // Client public key
    long long key;

    WSAStartup(MAKEWORD(2,2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server, sizeof(server));

    /* Receive public parameters */
    recv(sock, (char*)&p, sizeof(p), 0);
    recv(sock, (char*)&g, sizeof(g), 0);
    recv(sock, (char*)&Ya, sizeof(Ya), 0);

    printf("\n--- CLIENT SIDE ---\n");
    printf("Public Prime (p): %lld\n", p);
    printf("Public Generator (g): %lld\n", g);
    printf("Received Server Public Key (Ya): %lld\n", Ya);

    /* Client private key */
    printf("Enter Client Private Key (Xb): ");
    scanf("%lld", &Xb);

    Yb = modexp(g, Xb, p);
    printf("Computed Client Public Key (Yb = g^Xb mod p): %lld\n", Yb);

    /* Send only Yb */
    send(sock, (char*)&Yb, sizeof(Yb), 0);

    /* Compute shared key */
    key = modexp(Ya, Xb, p);
    printf("Client Shared Secret Key: %lld\n", key);

    closesocket(sock);
    WSACleanup();
    return 0;
}