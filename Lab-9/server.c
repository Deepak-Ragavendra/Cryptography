#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

int modInverse(int a,int p)
{
    int i;

    for(i=1;i<p;i++)
        if((a*i)%p==1)
            return i;

    return 1;
}

int main()
{
    WSADATA wsa;
    SOCKET s,new_socket;

    struct sockaddr_in server,client;

    int c;

    int p,k;

    int xs[10],ys[10];

    int i,j;

    int secret=0;

    WSAStartup(MAKEWORD(2,2),&wsa);

    s = socket(AF_INET , SOCK_STREAM , 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    bind(s,(struct sockaddr *)&server,sizeof(server));

    listen(s,3);

    printf("Server waiting...\n");

    c = sizeof(struct sockaddr_in);
    new_socket = accept(s,(struct sockaddr *)&client,&c);

    recv(new_socket,(char*)&p,sizeof(p),0);
    recv(new_socket,(char*)&k,sizeof(k),0);

    printf("\nReceived p = %d\n",p);
    printf("Received k = %d\n",k);

    printf("\nReceived Shares:\n");

    for(i=0;i<k;i++)
    {
        recv(new_socket,(char*)&xs[i],sizeof(int),0);
        recv(new_socket,(char*)&ys[i],sizeof(int),0);

        printf("(%d , %d)\n",xs[i],ys[i]);
    }

    /* Lagrange Interpolation */

    for(i=0;i<k;i++)
    {
        int num=1,den=1;

        for(j=0;j<k;j++)
        {
            if(i!=j)
            {
                num=(num*(-xs[j]))%p;
                den=(den*(xs[i]-xs[j]))%p;
            }
        }

        if(num<0) num+=p;
        if(den<0) den+=p;

        int inv = modInverse(den,p);

        int term = ys[i]*num%p*inv%p;

        secret=(secret+term)%p;
    }

    if(secret<0)
        secret+=p;

    printf("\nReconstructed Secret = %d\n",secret);

    closesocket(new_socket);
    WSACleanup();

    return 0;
}