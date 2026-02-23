#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    long long p, g, Yb;
    long long replayYa;   // Replayed CLIENT public key

    WSAStartup(MAKEWORD(2,2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server, sizeof(server));

    /* Receive public values from server */
    recv(sock, (char*)&p, sizeof(p), 0);
    recv(sock, (char*)&g, sizeof(g), 0);
    recv(sock, (char*)&Yb, sizeof(Yb), 0);

    printf("\n--- REPLAY ATTACKER ---\n");
    printf("Captured Prime (p): %lld\n", p);
    printf("Captured Generator (g): %lld\n", g);
    printf("Captured Server Public Key (Yb): %lld\n", Yb);

    /* Replay previously captured Ya */
    printf("Enter captured Client Public Key (Ya): ");
    scanf("%lld", &replayYa);

    send(sock, (char*)&replayYa, sizeof(replayYa), 0);

    printf("Old Ya replayed to server successfully.\n");
    printf("Server will compute a valid key again.\n");

    closesocket(sock);
    WSACleanup();
    return 0;
}