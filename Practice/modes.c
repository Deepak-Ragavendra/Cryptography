#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32.lib");

#define BLOCK 8

#define Mode_ECB 1
#define Mode_CBC 2
#define Mode_CFB 3
#define Mode_OFB 4
#define Mode_CTR 5

void xor_b(char *a,char *b,char *c){
    for(int i=0;i<BLOCK;i++){
        c[i]=a[i]^b[i];
    }
}

void ECB(char *d,int len,char *k){
    for(int i=0;i<len;i+=BLOCK){
        xor_b(d+i,k,d+i);
    }
}

void CBC(char *d,int len,char *iv,char *k){
    for(int i=0;i<len;i+=BLOCK){
        xor_b(d+i,iv,d+i);
        xor_b(d+i,k,d+i);
        memcpy(iv,d+i,BLOCK);
    }
}

void CFB(char *d,int len,char *iv,char *k){
    char s[BLOCK];
    for(int i=0;i<len;i+=BLOCK){
        xor_b(iv,k,s);
        xor_b(d+i,s,d+i);
        memcpy(iv,d+i,BLOCK);
    }
}

void OFB(char *d,int len,char *iv,char *k){
    char s[BLOCK];
    for(int i=0;i<len;i+=BLOCK){
        xor_b(iv,k,s);
        memcpy(iv,s,BLOCK);
        xor_b(d+i,s,d+i);
    }
}

void CTR(char *d,int len,char *k,char *ctr){
    char s[BLOCK];
    for(int i=0;i<len;i+=BLOCK){
        xor_b(k,ctr,s);
        xor_b(d+i,s,d+i);
        ctr[BLOCK-1]++;
    }
}

int padd_zeros(char *d,int len){
    int r=len%BLOCK;
    if(r==0) return len;
    memset(d+len,0,BLOCK-r);
    return len+(BLOCK-r);
}

int main(){
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    WSAStartup(MAKEWORD(2,2),&wsa);

    client_socket=socket(AF_INET,SOCK_STREAM,0);

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8080);
    server_addr.sin_addr.s_addr=inet_addr("10.73.240.144");

    connect(client_socket,(struct sockaddr*)&server_addr,sizeof(server_addr));

    char key[BLOCK+1] = "KEY12345";
    char iv[BLOCK+1]  = "INITVEC1";
    char ctr[BLOCK+1] = "COUNTER1";


    int mode;
    char data[128];
    printf("Enter the data in 0s and 1s: ");
    fgets(data,sizeof(data),stdin);
    data[strcspn(data,"\n")]=0;
    int  len=padd_zeros(data,strlen(data));

    printf("\n1.ECB  2.CBC  3.CFB  4.OFB  5.CTR\nChoose mode: ");
    scanf("%d", &mode);

    switch(mode){
        case Mode_ECB:ECB(data,len,key);
        case Mode_CBC:CBC(data,len,iv,key);
        case Mode_CFB:CFB(data,len,iv,key);
        case Mode_OFB:OFB(data,len,iv,key);
        case Mode_CTR:CTR(data,len,key,ctr);
    }
    printf("Cipher text: %s",data);

    send(client_socket,(char*)&mode,sizeof(mode),0);
    send(client_socket,(char*)&len,sizeof(len),0);
    send(client_socket,data,len,0);

    closesocket(client_socket);

    WSACleanup();
    return 0;


}