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

    WSAStartup(MAKEWORD(2,2), &wsa);

    server = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 3);

    printf("Server waiting...\n");
    client = accept(server, (struct sockaddr*)&addr, &addrlen);

    recv(client, (char*)&p, sizeof(p), 0);
    recv(client, (char*)&q, sizeof(q), 0);
    recv(client, (char*)&e, sizeof(e), 0);
    recv(client, (char*)&len, sizeof(len), 0);
    recv(client, (char*)encrypted, sizeof(long long) * len, 0);

    N = p * q;
    if (N <= 127) {
        printf("Error: N too small for ASCII decryption\n");
        return 0;
    }

    phi = (p - 1) * (q - 1);
    d = modInverse(e, phi);

    for(int i=0;i<len;i++) {
        printf("Received encrypted value: %lld\n", encrypted[i]);
    } 
    
    for (int i = 0; i < len; i++) {
        long long ascii = modexp(encrypted[i], d, N);
        decrypted[i] = (char)ascii;
        printf("Decrypted %lld to '%c'\n", ascii, decrypted[i]);
    }
    decrypted[len] = '\0';

    printf("Decrypted message: %s\n", decrypted);

    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}