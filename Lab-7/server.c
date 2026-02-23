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
    printf("Enter Prime Number (p): ");
    scanf("%lld", &p);

    printf("Enter Generator (g): ");
    scanf("%lld", &g);

    printf("Enter Server Private Key (Xb): ");
    scanf("%lld", &Xb);

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