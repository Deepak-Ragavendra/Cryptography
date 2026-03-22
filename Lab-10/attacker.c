#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"Ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    long long p, q, g, y, r, sig;
    char fakeMsg[256];
    char buffer[1024];

    printf("Attacker knows only public values and signature.\n");
    printf("Enter intercepted p: ");
    scanf("%lld", &p);
    printf("Enter intercepted q: ");
    scanf("%lld", &q);
    printf("Enter intercepted g: ");
    scanf("%lld", &g);
    printf("Enter intercepted y: ");
    scanf("%lld", &y);
    printf("Enter intercepted r: ");
    scanf("%lld", &r);
    printf("Enter intercepted s: ");
    scanf("%lld", &sig);

    getchar();
    printf("Enter modified fake message: ");
    fgets(fakeMsg, sizeof(fakeMsg), stdin);
    fakeMsg[strcspn(fakeMsg, "\n")] = '\0';

    WSAStartup(MAKEWORD(2,2), &wsa);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8080);

    connect(s, (struct sockaddr *)&server, sizeof(server));

    sprintf(buffer, "%lld\n%lld\n%lld\n%lld\n%lld\n%lld\n%s",
            p, q, g, y, r, sig, fakeMsg);

    send(s, buffer, strlen(buffer) + 1, 0);

    printf("\nAttacker sent modified message with old signature.\n");
    printf("Server should reject it.\n");

    closesocket(s);
    WSACleanup();

    return 0;
}