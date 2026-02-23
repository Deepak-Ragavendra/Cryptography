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
    SOCKET serverSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);

    long long p, g;
    long long Xb;   // Server private key
    long long Yb;   // Server public key
    long long Ya;   // Client public key
    long long sharedKey;

    WSAStartup(MAKEWORD(2,2), &wsa);

    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9090);   // Server port
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSock, 1);

    printf("Waiting for client...\n");
    clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &addrLen);

    printf("\n--- SERVER SIDE ---\n");

    /* Input public parameters */
    printf("Enter prime number (p): ");
    scanf("%lld", &p);

    printf("Enter generator (g): ");
    scanf("%lld", &g);

    printf("Enter Server Private Key (Xb): ");
    scanf("%lld", &Xb);

    /* Compute server public key */
    Yb = modexp(g, Xb, p);
    printf("Computed Server Public Key Yb = g^Xb mod p = %lld\n", Yb);

    /* Send public values to client */
    send(clientSock, (char*)&p, sizeof(p), 0);
    send(clientSock, (char*)&g, sizeof(g), 0);
    send(clientSock, (char*)&Yb, sizeof(Yb), 0);

    /* Receive client public key */
    recv(clientSock, (char*)&Ya, sizeof(Ya), 0);
    printf("Received Client Public Key Ya = %lld\n", Ya);

    /* Compute shared secret */
    sharedKey = modexp(Ya, Xb, p);
    printf("Server Shared Secret Key = (Ya^Xb) mod p = %lld\n", sharedKey);

    closesocket(clientSock);
    closesocket(serverSock);
    WSACleanup();
    return 0;
}