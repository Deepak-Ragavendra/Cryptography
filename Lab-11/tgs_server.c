/* tgs_server.c - Ticket Granting Server (TGS) - Port 9002
   Compile: gcc tgs_server.c -o tgs_server -lws2_32 */
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#define PORT     9002
#define KEY_LEN  32
#define TKT_LEN  128
#define AUTH_LEN 64
#define REQ_LEN  (TKT_LEN + 1 + AUTH_LEN)
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

    const char *tgsKey            = "tgs123";
    const char *serviceKey        = "srv123";
    const char *serviceSessionKey = "SK_C_SRV";

    WSAStartup(MAKEWORD(2, 2), &wsa);
    srv = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(PORT);
    bind(srv, (struct sockaddr *)&server, sizeof(server));
    listen(srv, 3);

    printf("[TGS] Waiting on port %d...\n\n", PORT);
    cli = accept(srv, (struct sockaddr *)&client, &clen);

    /* Receive request */
    unsigned char request[REQ_LEN];
    memset(request, 0, REQ_LEN);
    recvAll(cli, request, REQ_LEN);
    printf("[TGS] Request received from client.\n");

    if (request[TKT_LEN] != '#') {
        printf("[TGS] ERROR: Malformed request.\n");
        closesocket(cli); closesocket(srv); WSACleanup(); return 1;
    }

    /* Split TGT and authenticator */
    unsigned char encTGT[TKT_LEN], encAuth[AUTH_LEN];
    memcpy(encTGT,  request,               TKT_LEN);
    memcpy(encAuth, request + TKT_LEN + 1, AUTH_LEN);

    /* Decrypt TGT with tgsKey */
    unsigned char tgt[TKT_LEN];
    memcpy(tgt, encTGT, TKT_LEN);
    xorCrypt(tgt, TKT_LEN, tgsKey);

    char userFromTGT[49] = {0};
    char sk_as_tgs[TKT_LEN - 49] = {0};
    unsigned char *sep = (unsigned char *)memchr(tgt, '|', TKT_LEN);
    if (!sep) {
        printf("[TGS] ERROR: Corrupt TGT.\n");
        closesocket(cli); closesocket(srv); WSACleanup(); return 1;
    }
    int ulen = (int)(sep - tgt); if (ulen > 48) ulen = 48;
    memcpy(userFromTGT, tgt, ulen);
    strncpy(sk_as_tgs, (char *)sep + 1, sizeof(sk_as_tgs) - 1);
    printf("[TGS] TGT decrypted       : user=%s, SK_AS_TGS=%s\n", userFromTGT, sk_as_tgs);

    /* Decrypt authenticator with SK_AS_TGS */
    unsigned char auth[AUTH_LEN];
    memcpy(auth, encAuth, AUTH_LEN);
    xorCrypt(auth, AUTH_LEN, sk_as_tgs);
    char userFromAuth[49] = {0};
    strncpy(userFromAuth, (char *)auth, 48);
    printf("[TGS] Authenticator       : user=%s\n", userFromAuth);

    /* Verify */
    if (strcmp(userFromTGT, userFromAuth) != 0) {
        printf("[TGS] FAILED: Username mismatch!\n");
        closesocket(cli); closesocket(srv); WSACleanup(); return 1;
    }
    printf("[TGS] Identity verified   : OK\n");

    /* Build service ticket: "username|SK_C_SRV" encrypted with serviceKey */
    unsigned char svcTicket[TKT_LEN];
    memset(svcTicket, 0, TKT_LEN);
    strncpy((char *)svcTicket,      userFromTGT,       47);
    svcTicket[47] = '|';
    strncpy((char *)svcTicket + 48, serviceSessionKey, TKT_LEN - 48 - 1);
    printf("[TGS] Service ticket      : %s|%s\n", userFromTGT, serviceSessionKey);
    xorCrypt(svcTicket, TKT_LEN, serviceKey);
    printf("[TGS] Ticket encrypted with: serviceKey (\"%s\")\n", serviceKey);

    /* Build response: "SK_C_SRV # ServiceTicket" encrypted with SK_AS_TGS */
    unsigned char response[RESP_LEN];
    memset(response, 0, RESP_LEN);
    strncpy((char *)response, serviceSessionKey, KEY_LEN - 1);
    response[KEY_LEN] = '#';
    memcpy(response + KEY_LEN + 1, svcTicket, TKT_LEN);
    printf("[TGS] Sending session key : %s\n", serviceSessionKey);
    xorCrypt(response, RESP_LEN, sk_as_tgs);
    printf("[TGS] Response encrypted with: SK_AS_TGS (\"%s\")\n", sk_as_tgs);
    printf("[TGS] Response sent to client.\n");

    sendAll(cli, response, RESP_LEN);

    closesocket(cli);
    closesocket(srv);
    WSACleanup();
    return 0;
}