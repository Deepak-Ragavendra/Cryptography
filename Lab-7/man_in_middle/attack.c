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
    SOCKET fakeServer, clientSock, serverSock;
    struct sockaddr_in addr, server;

    long long p, g;
    long long Ya, Yb;

    /* Attacker private keys */
    long long Xm1 = 5;   // For Server
    long long Xm2 = 7;   // For Client

    long long Ym1, Ym2;
    long long keyWithServer, keyWithClient;

    WSAStartup(MAKEWORD(2,2), &wsa);

    /* Step 1: Act as fake server to client */
    fakeServer = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(fakeServer, (struct sockaddr*)&addr, sizeof(addr));
    listen(fakeServer, 1);

    printf("\n[MITM] Waiting for Client...\n");
    clientSock = accept(fakeServer, NULL, NULL);

    /* Step 2: Connect to real server */
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(serverSock, (struct sockaddr*)&server, sizeof(server));

    /* Step 3: Receive public values from server */
    recv(serverSock, (char*)&p, sizeof(p), 0);
    recv(serverSock, (char*)&g, sizeof(g), 0);
    recv(serverSock, (char*)&Ya, sizeof(Ya), 0);

    printf("\n--- INTERCEPTED FROM SERVER ---\n");
    printf("Prime (p) = %lld\n", p);
    printf("Generator (g) = %lld\n", g);
    printf("Server Public Key (Ya = g^Xa mod p) = %lld\n", Ya);

    /* Step 4: Attacker sends fake public key to client */
    Ym2 = modexp(g, Xm2, p);

    printf("\n--- ATTACKER to CLIENT ---\n");
    printf("Attacker Private Key Xm2 = %lld\n", Xm2);
    printf("Computed Ym2 = g^Xm2 mod p = %lld\n", Ym2);

    send(clientSock, (char*)&p, sizeof(p), 0);
    send(clientSock, (char*)&g, sizeof(g), 0);
    send(clientSock, (char*)&Ym2, sizeof(Ym2), 0);

    /* Step 5: Receive Yb from client */
    recv(clientSock, (char*)&Yb, sizeof(Yb), 0);

    printf("\n--- INTERCEPTED FROM CLIENT ---\n");
    printf("Client Public Key (Yb = g^Xb mod p) = %lld\n", Yb);

    /* Step 6: Attacker sends fake public key to server */
    Ym1 = modexp(g, Xm1, p);

    printf("\n--- ATTACKER to SERVER ---\n");
    printf("Attacker Private Key Xm1 = %lld\n", Xm1);
    printf("Computed Ym1 = g^Xm1 mod p = %lld\n", Ym1);

    send(serverSock, (char*)&Ym1, sizeof(Ym1), 0);

    /* Step 7: Attacker computes BOTH shared keys */
    keyWithClient = modexp(Yb, Xm2, p);
    keyWithServer = modexp(Ya, Xm1, p);

    printf("\n=== SHARED KEYS CALCULATED BY ATTACKER ===\n");
    printf("Key with Client = (Yb ^ Xm2) mod p = %lld\n", keyWithClient);
    printf("Key with Server = (Ya ^ Xm1) mod p = %lld\n", keyWithServer);

    printf("\n[MITM SUCCESS] Attacker knows both keys!\n");

    closesocket(clientSock);
    closesocket(serverSock);
    closesocket(fakeServer);
    WSACleanup();
    return 0;
}