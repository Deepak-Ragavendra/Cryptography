/* client.c - Kerberos Client
   Compile: gcc client.c -o client -lws2_32 */
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#define AS_PORT  9001
#define TGS_PORT 9002
#define SRV_PORT 9003

#define KEY_LEN      32
#define TKT_LEN      128
#define AUTH_LEN     64
#define AS_RESP_LEN  (KEY_LEN + 1 + TKT_LEN)
#define TGS_REQ_LEN  (TKT_LEN + 1 + AUTH_LEN)
#define TGS_RESP_LEN (KEY_LEN + 1 + TKT_LEN)
#define SS_REQ_LEN   (TKT_LEN + 1 + AUTH_LEN)

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

SOCKET connectTo(int port) {
    SOCKET s;
    struct sockaddr_in srv;
    s = socket(AF_INET, SOCK_STREAM, 0);
    srv.sin_family      = AF_INET;
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv.sin_port        = htons(port);
    if (connect(s, (struct sockaddr *)&srv, sizeof(srv)) == SOCKET_ERROR) {
        printf("[Client] Cannot connect to port %d\n", port);
        closesocket(s); return INVALID_SOCKET;
    }
    return s;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    const char *userKey = "user123";

    char username[49] = {0};
    printf("Enter username: ");
    scanf("%48s", username);

    /* ---- Step 1: Contact AS ---- */
    printf("\n[Client] Contacting AS...\n");
    unsigned char userBuf[50] = {0};
    strncpy((char *)userBuf, username, 48);

    SOCKET s = connectTo(AS_PORT);
    if (s == INVALID_SOCKET) { WSACleanup(); return 1; }
    sendAll(s, userBuf, sizeof(userBuf));

    unsigned char asResp[AS_RESP_LEN];
    recvAll(s, asResp, AS_RESP_LEN);
    closesocket(s);

    xorCrypt(asResp, AS_RESP_LEN, userKey);
    if (asResp[KEY_LEN] != '#') {
        printf("[Client] ERROR: Bad AS response.\n");
        WSACleanup(); return 1;
    }

    char sk_as_tgs[KEY_LEN + 1] = {0};
    memcpy(sk_as_tgs, asResp, KEY_LEN);

    unsigned char tgt[TKT_LEN];
    memcpy(tgt, asResp + KEY_LEN + 1, TKT_LEN);

    printf("[Client] Got session key from AS : %s\n", sk_as_tgs);
    printf("[Client] Got TGT from AS         : <encrypted, cannot read>\n");

    /* ---- Step 2: Contact TGS ---- */
    printf("\n[Client] Contacting TGS...\n");

    unsigned char auth1[AUTH_LEN] = {0};
    strncpy((char *)auth1, username, AUTH_LEN - 1);
    xorCrypt(auth1, AUTH_LEN, sk_as_tgs);

    unsigned char tgsReq[TGS_REQ_LEN];
    memcpy(tgsReq,              tgt,   TKT_LEN);
    tgsReq[TKT_LEN] = '#';
    memcpy(tgsReq + TKT_LEN + 1, auth1, AUTH_LEN);

    s = connectTo(TGS_PORT);
    if (s == INVALID_SOCKET) { WSACleanup(); return 1; }
    sendAll(s, tgsReq, TGS_REQ_LEN);

    unsigned char tgsResp[TGS_RESP_LEN];
    recvAll(s, tgsResp, TGS_RESP_LEN);
    closesocket(s);

    xorCrypt(tgsResp, TGS_RESP_LEN, sk_as_tgs);
    if (tgsResp[KEY_LEN] != '#') {
        printf("[Client] ERROR: Bad TGS response.\n");
        WSACleanup(); return 1;
    }

    char sk_c_srv[KEY_LEN + 1] = {0};
    memcpy(sk_c_srv, tgsResp, KEY_LEN);

    unsigned char svcTicket[TKT_LEN];
    memcpy(svcTicket, tgsResp + KEY_LEN + 1, TKT_LEN);

    printf("[Client] Got session key from TGS: %s\n", sk_c_srv);
    printf("[Client] Got service ticket       : <encrypted, cannot read>\n");

    /* ---- Step 3: Contact Service Server ---- */
    printf("\n[Client] Contacting Service Server...\n");

    unsigned char auth2[AUTH_LEN] = {0};
    strncpy((char *)auth2, username, AUTH_LEN - 1);
    xorCrypt(auth2, AUTH_LEN, sk_c_srv);

    unsigned char ssReq[SS_REQ_LEN];
    memcpy(ssReq,                svcTicket, TKT_LEN);
    ssReq[TKT_LEN] = '#';
    memcpy(ssReq + TKT_LEN + 1, auth2,     AUTH_LEN);

    s = connectTo(SRV_PORT);
    if (s == INVALID_SOCKET) { WSACleanup(); return 1; }
    sendAll(s, ssReq, SS_REQ_LEN);

    char reply[64] = {0};
    recv(s, reply, sizeof(reply) - 1, 0);
    closesocket(s);

    printf("\n[Client] Server reply: %s\n", reply);

    WSACleanup();
    return 0;
}