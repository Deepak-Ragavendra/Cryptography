#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"Ws2_32.lib")

long long modexp(long long a, long long b, long long m)
{
    long long res = 1;
    a = a % m;

    while (b > 0)
    {
        if (b % 2 == 1)
            res = (res * a) % m;

        a = (a * a) % m;
        b = b / 2;
    }

    return res;
}

long long modInverse(long long a, long long m)
{
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        q = a / m;
        t = m;
        m = a % m;
        a = t;

        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
        x1 += m0;

    return x1;
}

long long hashMessage(char msg[], long long q)
{
    long long h = 0;
    int i;

    for (i = 0; msg[i] != '\0'; i++)
        h = (h + msg[i]) % q;

    return h;
}

int main()
{
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c;

    char buffer[1024];
    char result[100];

    long long p, q, g, y, r, sig;
    long long hm, w, u1, u2, v;
    char msg[256];

    WSAStartup(MAKEWORD(2, 2), &wsa);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9090);   // separate port for attacker check

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 3);

    printf("Attacker Verification Server waiting on port 9090...\n");

    c = sizeof(struct sockaddr_in);
    new_socket = accept(s, (struct sockaddr *)&client, &c);

    recv(new_socket, buffer, sizeof(buffer), 0);

    sscanf(buffer, "%lld\n%lld\n%lld\n%lld\n%lld\n%lld\n%[^\n]",
           &p, &q, &g, &y, &r, &sig, msg);

    printf("\nReceived from attacker:\n");
    printf("p = %lld\n", p);
    printf("q = %lld\n", q);
    printf("g = %lld\n", g);
    printf("y = %lld\n", y);
    printf("r = %lld\n", r);
    printf("s = %lld\n", sig);
    printf("Modified Message = %s\n", msg);

    hm = hashMessage(msg, q);

    w = modInverse(sig, q);
    u1 = (hm * w) % q;
    u2 = (r * w) % q;
    v = ((modexp(g, u1, p) * modexp(y, u2, p)) % p) % q;

    printf("\nVerification at attacker server:\n");
    printf("H(m) = %lld\n", hm);
    printf("w    = %lld\n", w);
    printf("u1   = %lld\n", u1);
    printf("u2   = %lld\n", u2);
    printf("v    = %lld\n", v);

    if (v == r)
    {
        printf("\nValid digital signature\n");
        strcpy(result, "Valid digital signature");
    }
    else
    {
        printf("\nNot a valid digital signature\n");
        strcpy(result, "Not a valid digital signature");
    }

    send(new_socket, result, strlen(result) + 1, 0);

    closesocket(new_socket);
    closesocket(s);
    WSACleanup();

    return 0;
}