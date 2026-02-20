// dh_server.c
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
    SOCKET sfd, cfd;
    struct sockaddr_in server, client;
    int c = sizeof(client);

    long long p, g;
    long long Xa;   // Server private key
    long long Ya;   // Server public key
    long long Yb;   // Client public key
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
    printf("Enter Prime Number (p): ");
    scanf("%lld", &p);

    printf("Enter Generator (g): ");
    scanf("%lld", &g);

    printf("Enter Server Private Key (Xa): ");
    scanf("%lld", &Xa);

    Ya = modexp(g, Xa, p);
    printf("Computed Server Public Key (Ya = g^Xa mod p): %lld\n", Ya);

    /* Send only public values */
    send(cfd, (char*)&p, sizeof(p), 0);
    send(cfd, (char*)&g, sizeof(g), 0);
    send(cfd, (char*)&Ya, sizeof(Ya), 0);

    /* Receive client public key */
    recv(cfd, (char*)&Yb, sizeof(Yb), 0);
    printf("Received Client Public Key (Yb): %lld\n", Yb);

    /* Compute shared key */
    key = modexp(Yb, Xa, p);
    printf("Server Shared Secret Key: %lld\n", key);

    closesocket(cfd);
    closesocket(sfd);
    WSACleanup();
    return 0;
}