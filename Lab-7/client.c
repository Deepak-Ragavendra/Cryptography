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
    long long Xa;   // ✅ Client private key
    long long Ya;   // ✅ Client public key
    long long Yb;   // Server public key
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
    recv(sock, (char*)&Yb, sizeof(Yb), 0);

    printf("\n--- CLIENT SIDE ---\n");
    printf("Public Prime (p): %lld\n", p);
    printf("Public Generator (g): %lld\n", g);
    printf("Received Server Public Key (Yb): %lld\n", Yb);

    /* Client private key */
    printf("Enter Client Private Key (Xa): ");
    scanf("%lld", &Xa);

    Ya = modexp(g, Xa, p);
    printf("Computed Client Public Key (Ya = g^Xa mod p): %lld\n", Ya);

    /* Send Ya */
    send(sock, (char*)&Ya, sizeof(Ya), 0);

    /* Compute shared key */
    key = modexp(Yb, Xa, p);
    printf("Client Shared Secret Key = (Yb^Xa) mod p = %lld\n", key);

    closesocket(sock);
    WSACleanup();
    return 0;
}