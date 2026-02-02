#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

void left_shift(int key[10], int array[5], int k){
    if(k==0){
        int t=key[0];
        for(int i=1;i<5;i++) array[i-1]=key[i];
        array[4]=t;
    } else {
        int t=key[5];
        for(int i=6;i<10;i++) array[i-6]=key[i];
        array[4]=t;
    }
}

int main(){
    WSADATA wsadata;
    SOCKET server, client;
    struct sockaddr_in addr, cli;
    int len=sizeof(cli);
    WSAStartup(MAKEWORD(2,2),&wsadata);

    server=socket(AF_INET,SOCK_STREAM,0);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(4000);
    addr.sin_addr.S_un.S_addr=INADDR_ANY;
    bind(server,(struct sockaddr*)&addr,sizeof(addr));
    listen(server,1);
    printf("Waiting for client...\n");
    client=accept(server,(struct sockaddr*)&cli,&len);

    int original_len,padded_len;
    recv(client,(char*)&original_len,sizeof(int),0);
    recv(client,(char*)&padded_len,sizeof(int),0);

    int cipher[128];
    recv(client,(char*)cipher,padded_len*sizeof(int),0);

    // -------- KEY GENERATION (same as client) --------
    int P10[]={3,5,2,7,4,10,1,9,8,6};
    int P8[]={6,3,7,4,8,5,10,9};
    int key[]={1,0,1,0,0,0,0,0,1,0};
    int temp[10], L[5], R[5], newkey[10];
    for(int i=0;i<10;i++) temp[i]=key[P10[i]-1];
    left_shift(temp,L,0);
    left_shift(temp,R,1);
    for(int i=0;i<5;i++){ newkey[i]=L[i]; newkey[i+5]=R[i]; }
    int k1[8];
    for(int i=0;i<8;i++) k1[i]=newkey[P8[i]-1];
    left_shift(L,L,0); left_shift(R,R,1);
    left_shift(L,L,0); left_shift(R,R,1);
    for(int i=0;i<5;i++){ newkey[i]=L[i]; newkey[i+5]=R[i]; }
    int k2[8];
    for(int i=0;i<8;i++) k2[i]=newkey[P8[i]-1];

    // -------- DECRYPT --------
    int S0[4][4]={{1,0,3,2},{3,2,1,0},{0,2,1,3},{3,1,3,2}};
    int S1[4][4]={{0,1,2,3},{2,0,1,3},{3,0,1,0},{2,1,0,3}};
    int IP[8]={2,6,3,1,4,8,5,7};
    int EP[8]={4,1,2,3,2,3,4,1};
    int P4[4]={2,4,3,1};
    int IP_inv[8]={4,1,3,5,7,2,8,6};

    int PT[128], pi=0;

    for(int b=0;b<padded_len;b+=8){
        int block[8], IP_out[8], L4[4], R4[4];
        for(int i=0;i<8;i++) block[i]=cipher[b+i];
        for(int i=0;i<8;i++) IP_out[i]=block[IP[i]-1];
        for(int i=0;i<4;i++){ L4[i]=IP_out[i]; R4[i]=IP_out[i+4]; }

        int EP_out[8], xor_k[8], NL[4], NR[4];
        for(int i=0;i<8;i++) EP_out[i]=R4[EP[i]-1];
        for(int i=0;i<8;i++){
            xor_k[i]=k2[i]^EP_out[i];
            if(i<4) NL[i]=xor_k[i];
            else NR[i-4]=xor_k[i];
        }

        int r1=NL[0]*2+NL[3], c1=NL[1]*2+NL[2];
        int r2=NR[0]*2+NR[3], c2=NR[1]*2+NR[2];
        int s_out[4]={S0[r1][c1]/2,S0[r1][c1]%2,S1[r2][c2]/2,S1[r2][c2]%2};
        int p4[4];
        for(int i=0;i<4;i++) p4[i]=s_out[P4[i]-1]^L4[i];
        for(int i=0;i<4;i++){ L4[i]=R4[i]; R4[i]=p4[i]; }

        for(int i=0;i<8;i++) EP_out[i]=R4[EP[i]-1];
        for(int i=0;i<8;i++){
            xor_k[i]=k1[i]^EP_out[i];
            if(i<4) NL[i]=xor_k[i];
            else NR[i-4]=xor_k[i];
        }

        r1=NL[0]*2+NL[3]; c1=NL[1]*2+NL[2];
        r2=NR[0]*2+NR[3]; c2=NR[1]*2+NR[2];
        s_out[0]=S0[r1][c1]/2; s_out[1]=S0[r1][c1]%2;
        s_out[2]=S1[r2][c2]/2; s_out[3]=S1[r2][c2]%2;
        for(int i=0;i<4;i++) p4[i]=s_out[P4[i]-1]^L4[i];

        int pre[8];
        for(int i=0;i<4;i++){ pre[i]=p4[i]; pre[i+4]=R4[i]; }
        for(int i=0;i<8;i++) PT[pi++]=pre[IP_inv[i]-1];
    }

    printf("Plain Text: ");
    for(int i=0;i<original_len;i++) printf("%d",PT[i]);
    printf("\n");

    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}
