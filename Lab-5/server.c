// server.c
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

void xor_block(char *a, char *b, char *o) {
    for (int i = 0; i < BLOCK; i++)
        o[i] = a[i] ^ b[i];
}

void ECB(char *d, int len, char *k) {
    for (int i = 0; i < len; i += BLOCK)
        xor_block(d+i, k, d+i);
}

void CBC_dec(char *d, int len, char *k, char *iv) {
    char prev[BLOCK], tmp[BLOCK];
    for (int i = 0; i < len; i += BLOCK) {
        memcpy(prev, d+i, BLOCK);
        xor_block(d+i, k, tmp);
        xor_block(tmp, iv, d+i);
        memcpy(iv, prev, BLOCK);
    }
}

void CFB_dec(char *d, int len, char *k, char *iv) {
    char s[BLOCK], c[BLOCK];
    for (int i = 0; i < len; i += BLOCK) {
        memcpy(c, d+i, BLOCK);
        xor_block(iv, k, s);
        xor_block(d+i, s, d+i);
        memcpy(iv, c, BLOCK);
    }
}

void OFB_CTR(char *d, int len, char *k, char *v, int ctr) {
    char s[BLOCK];
    for (int i = 0; i < len; i += BLOCK) {
        xor_block(v, k, s);
        if (ctr) v[BLOCK-1]++;
        else memcpy(v, s, BLOCK);
        xor_block(d+i, s, d+i);
    }
}

int main() {
    WSADATA wsa;
    SOCKET sfd, cfd;
    struct sockaddr_in addr;

    char buf[128];
    char key[BLOCK+1] = "KEY12345";
    char iv[BLOCK+1]  = "INITVEC1";
    char ctr[BLOCK+1] = "COUNTER1";

    int mode, len;

    WSAStartup(MAKEWORD(2,2), &wsa);

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sfd, 1);

    printf("Server waiting...\n");
    cfd = accept(sfd, NULL, NULL);

    recv(cfd, (char*)&mode, sizeof(mode), 0);
    recv(cfd, (char*)&len, sizeof(len), 0);
    recv(cfd, buf, len, 0);

    switch(mode) {
        case MODE_ECB: ECB(buf, len, key); break;
        case MODE_CBC: CBC_dec(buf, len, key, iv); break;
        case MODE_CFB: CFB_dec(buf, len, key, iv); break;
        case MODE_OFB: OFB_CTR(buf, len, key, iv, 0); break;
        case MODE_CTR: OFB_CTR(buf, len, key, ctr, 1); break;
    }

    buf[len] = '\0';
    printf("Decrypted data: %s\n", buf);

    closesocket(cfd);
    closesocket(sfd);
    WSACleanup();
    return 0;
}
