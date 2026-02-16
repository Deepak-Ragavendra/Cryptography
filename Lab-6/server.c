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

long long modInverse(long long e, long long phi) {
    long long t = 0, newt = 1;
    long long r = phi, newr = e;

    while (newr != 0) {
        long long q = r / newr;
        long long temp = newt;
        newt = t - q * newt;
        t = temp;

        temp = newr;
        newr = r - q * newr;
        r = temp;
    }
    if (t < 0)
        t += phi;
    return t;
}

int main() {
    WSADATA wsa;
    SOCKET server, client;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);

    long long p, q, e, N, phi, d;
    int len;
    long long encrypted[100];
    char decrypted[100];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }

    listen(server, 3);

    printf("Server waiting...\n");

    client = accept(server, (struct sockaddr*)&addr, &addrlen);
    if (client == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(server);
        WSACleanup();
        return 1;
    }


    // Receive p, q, e, len, and encrypted data from client
    recv(client, (char*)&p, sizeof(p), 0);
    recv(client, (char*)&q, sizeof(q), 0);
    recv(client, (char*)&e, sizeof(e), 0);
    recv(client, (char*)&len, sizeof(len), 0);
    
    if (len <= 0 || len >= 100) {
        printf("Invalid message length: %d\n", len);
        closesocket(client);
        closesocket(server);
        WSACleanup();
        return 1;
    }
    
    recv(client, (char*)encrypted, sizeof(long long) * len, 0);

    N = p * q;
    printf("Received p=%lld, q=%lld, e=%lld\n", p, q, e);
    printf("Calculated N=%lld, len=%d\n", N, len);

    phi = (p - 1) * (q - 1);
    d = modInverse(e, phi);

    printf("Private key d=%lld\n\n", d);

    for(int i = 0; i < len; i++) {
        printf("Received encrypted value: %lld\n", encrypted[i]);
    }
    
    printf("\nDecrypting...\n");
    for (int i = 0; i < len; i++) {
        long long ascii = modexp(encrypted[i], d, N);
        decrypted[i] = (char)ascii;
        printf("Decrypted %lld to '%c' (ASCII %lld)\n", encrypted[i], decrypted[i], ascii);
    }
    decrypted[len] = '\0';

    printf("\nDecrypted message: %s \n", decrypted);

    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}