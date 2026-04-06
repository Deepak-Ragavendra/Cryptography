/* auth_server.c - Authentication Server (AS) - Port 9001
   Compile: gcc auth_server.c -o auth_server -lws2_32 */
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#define PORT     9001
#define KEY_LEN  32
#define TKT_LEN  128
#define RESP_LEN (KEY_LEN + 1 + TKT_LEN)

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

    const char *userKey    = "user123";
    const char *tgsKey     = "tgs123";
    const char *sessionKey = "SK_AS_TGS";

    WSAStartup(MAKEWORD(2, 2), &wsa);
    srv = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(PORT);
    bind(srv, (struct sockaddr *)&server, sizeof(server));
    listen(srv, 3);

    printf("[AS] Waiting on port %d...\n\n", PORT);
    cli = accept(srv, (struct sockaddr *)&client, &clen);

    /* Receive username */
    unsigned char user[50];
    memset(user, 0, sizeof(user));
    recvAll(cli, user, sizeof(user));
    printf("[AS] Received username    : %s\n", (char *)user);

    /* Build TGT: "username|SK_AS_TGS" encrypted with tgsKey */
    unsigned char tgt[TKT_LEN];
    memset(tgt, 0, TKT_LEN);
    strncpy((char *)tgt,      (char *)user, 48);
    tgt[48] = '|';
    strncpy((char *)tgt + 49, sessionKey, TKT_LEN - 49 - 1);
    printf("[AS] TGT plaintext        : %s|%s\n", (char *)user, sessionKey);
    xorCrypt(tgt, TKT_LEN, tgsKey);
    printf("[AS] TGT encrypted with   : tgsKey (\"%s\")\n", tgsKey);

    /* Build response: "SK_AS_TGS # TGT" encrypted with userKey */
    unsigned char response[RESP_LEN];
    memset(response, 0, RESP_LEN);
    strncpy((char *)response, sessionKey, KEY_LEN - 1);
    response[KEY_LEN] = '#';
    memcpy(response + KEY_LEN + 1, tgt, TKT_LEN);
    printf("[AS] Sending session key  : %s\n", sessionKey);
    xorCrypt(response, RESP_LEN, userKey);
    printf("[AS] Response encrypted with: userKey (\"%s\")\n", userKey);
    printf("[AS] Response sent to client.\n");

    sendAll(cli, response, RESP_LEN);

    closesocket(cli);
    closesocket(srv);
    WSACleanup();
    return 0;
}