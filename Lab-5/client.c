// client.c
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#define PORT 8080
#define BLOCK 8

#define MODE_ECB 1
#define MODE_CBC 2
#define MODE_CFB 3
#define MODE_OFB 4
#define MODE_CTR 5

/* ---------- HELPERS ---------- */
void xor_block(char *a, char *b, char *o) {
    for (int i = 0; i < BLOCK; i++)
        o[i] = a[i] ^ b[i];
}

int pad_zero(char *d, int len) {
    int r = len % BLOCK;
    if (r == 0) return len;
    memset(d + len, 0, BLOCK - r);
    return len + (BLOCK - r);
}

/* ---------- MODES ---------- */
void ECB(char *d, int len, char *k) {
    for (int i = 0; i < len; i += BLOCK)
        xor_block(d+i, k, d+i);
}

void CBC(char *d, int len, char *k, char *iv) {
    for (int i = 0; i < len; i += BLOCK) {
        xor_block(d+i, iv, d+i);
        xor_block(d+i, k, d+i);
        memcpy(iv, d+i, BLOCK);
    }
}

void CFB(char *d, int len, char *k, char *iv) {
    char s[BLOCK];
    for (int i = 0; i < len; i += BLOCK) {
        xor_block(iv, k, s);
        xor_block(d+i, s, d+i);
        memcpy(iv, d+i, BLOCK);
    }
}

void OFB(char *d, int len, char *k, char *iv) {
    char s[BLOCK];
    for (int i = 0; i < len; i += BLOCK) {
        xor_block(iv, k, s);
        memcpy(iv, s, BLOCK);
        xor_block(d+i, s, d+i);
    }
}

void CTR(char *d, int len, char *k, char *ctr) {
    char s[BLOCK];
    for (int i = 0; i < len; i += BLOCK) {
        xor_block(ctr, k, s);
        xor_block(d+i, s, d+i);
        ctr[BLOCK-1]++;
    }
}

/* ---------- MAIN ---------- */
int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    char data[128];
    char key[BLOCK+1] = "KEY12345";
    char iv[BLOCK+1]  = "INITVEC1";
    char ctr[BLOCK+1] = "COUNTER1";

    int mode;

    WSAStartup(MAKEWORD(2,2), &wsa);

    printf("Enter data (0s and 1s): ");
    fgets(data, sizeof(data), stdin);
    data[strcspn(data, "\n")] = 0;

    printf("\n1.ECB  2.CBC  3.CFB  4.OFB  5.CTR\nChoose mode: ");
    scanf("%d", &mode);

    int len = pad_zero(data, strlen(data));

    switch(mode) {
        case MODE_ECB: ECB(data, len, key); break;
        case MODE_CBC: CBC(data, len, key, iv); break;
        case MODE_CFB: CFB(data, len, key, iv); break;
        case MODE_OFB: OFB(data, len, key, iv); break;
        case MODE_CTR: CTR(data, len, key, ctr); break;
        default: printf("Invalid mode\n"); return 0;
    }
    printf("Cipher Text: %s\n",data);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&server, sizeof(server));

    send(sock, (char*)&mode, sizeof(mode), 0);
    send(sock, (char*)&len, sizeof(len), 0);
    send(sock, data, len, 0);

    closesocket(sock);
    WSACleanup();
    return 0;
}
