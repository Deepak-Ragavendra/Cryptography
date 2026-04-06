/* service_server.c - Service Server (SS) - Port 9003
   Compile: gcc service_server.c -o service_server -lws2_32 */
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#define PORT     9003
#define TKT_LEN  128
#define AUTH_LEN 64
#define REQ_LEN  (TKT_LEN + 1 + AUTH_LEN)

void xorCrypt(unsigned char *data, int len, const char *key) {
    int klen = (int)strlen(key);
    for (int i = 0; i < len; i++)
        data[i] ^= (unsigned char)key[i % klen];
}

int sendAll(SOCKET sock, const unsigned char *buf, int len) {
    int total = 0;
    while (total < len) {
        int n = send(sock, (const char *)buf + total, len - total, 0);
        if (n == SOCKET_ERROR) return -1;
        total += n;
    }
    return total;
}

int recvAll(SOCKET sock, unsigned char *buf, int len) {
    int total = 0;
    while (total < len) {
        int n = recv(sock, (char *)buf + total, len - total, 0);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}

int main() {
    WSADATA wsa;
    SOCKET srv, cli;
    struct sockaddr_in server, client;
    int clen = sizeof(client);

    const char *serviceKey = "srv123";

    WSAStartup(MAKEWORD(2, 2), &wsa);
    srv = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(PORT);
    bind(srv, (struct sockaddr *)&server, sizeof(server));
    listen(srv, 3);

    printf("[SS] Waiting on port %d...\n\n", PORT);
    cli = accept(srv, (struct sockaddr *)&client, &clen);

    /* Receive request */
    unsigned char request[REQ_LEN];
    memset(request, 0, REQ_LEN);
    recvAll(cli, request, REQ_LEN);
    printf("[SS] Request received from client.\n");

    if (request[TKT_LEN] != '#') {
        printf("[SS] ERROR: Malformed request.\n");
        closesocket(cli); closesocket(srv); WSACleanup(); return 1;
    }

    /* Split ticket and authenticator */
    unsigned char encTicket[TKT_LEN], encAuth[AUTH_LEN];
    memcpy(encTicket, request,               TKT_LEN);
    memcpy(encAuth,   request + TKT_LEN + 1, AUTH_LEN);

    /* Decrypt service ticket with serviceKey */
    unsigned char ticket[TKT_LEN];
    memcpy(ticket, encTicket, TKT_LEN);
    xorCrypt(ticket, TKT_LEN, serviceKey);

    char userFromTicket[49] = {0};
    char sk_c_srv[TKT_LEN - 49] = {0};
    unsigned char *sep = (unsigned char *)memchr(ticket, '|', TKT_LEN);
    if (!sep) {
        printf("[SS] ERROR: Corrupt ticket.\n");
        closesocket(cli); closesocket(srv); WSACleanup(); return 1;
    }
    int ulen = (int)(sep - ticket); if (ulen > 48) ulen = 48;
    memcpy(userFromTicket, ticket, ulen);
    strncpy(sk_c_srv, (char *)sep + 1, sizeof(sk_c_srv) - 1);
    printf("[SS] Ticket decrypted     : user=%s, SK_C_SRV=%s\n", userFromTicket, sk_c_srv);

    /* Decrypt authenticator with SK_C_SRV */
    unsigned char auth[AUTH_LEN];
    memcpy(auth, encAuth, AUTH_LEN);
    xorCrypt(auth, AUTH_LEN, sk_c_srv);
    char userFromAuth[49] = {0};
    strncpy(userFromAuth, (char *)auth, 48);
    printf("[SS] Authenticator        : user=%s\n", userFromAuth);

    /* Verify and respond */
    const char *reply;
    if (strcmp(userFromTicket, userFromAuth) == 0) {
        reply = "SERVICE_GRANTED";
        printf("[SS] Identity verified    : OK\n");
        printf("[SS] Access GRANTED to %s\n", userFromTicket);
    } else {
        reply = "SERVICE_DENIED";
        printf("[SS] FAILED: Username mismatch!\n");
        printf("[SS] Access DENIED.\n");
    }

    sendAll(cli, (const unsigned char *)reply, (int)strlen(reply) + 1);

    closesocket(cli);
    closesocket(srv);
    WSACleanup();
    return 0;
}