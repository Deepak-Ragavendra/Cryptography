#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

long long modexp(long long a,long long b,long long p)
{
    long long res=1;
    a%=p;

    while(b>0)
    {
        if(b%2==1)
            res=(res*a)%p;

        b/=2;
        a=(a*a)%p;
    }

    return res;
}

long long modInverse(long long a,long long p)
{
    return modexp(a,p-2,p);
}

int main()
{
    WSADATA wsa;
    SOCKET s,new_socket;
    struct sockaddr_in server,client;

    int c;

    long long HC1,HC2;
    long long x,p;

    long long svalue,inv,m;

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

    recv(new_socket,(char*)&HC1,sizeof(HC1),0);
    recv(new_socket,(char*)&HC2,sizeof(HC2),0);

    recv(new_socket,(char*)&x,sizeof(x),0);
    recv(new_socket,(char*)&p,sizeof(p),0);

    printf("\nReceived Cipher = (%lld , %lld)\n",HC1,HC2);

    svalue = modexp(HC1,x,p);

    inv = modInverse(svalue,p);

    m = (HC2 * inv)%p;

    printf("Decrypted Value = %lld\n",m);

    closesocket(new_socket);
    WSACleanup();

    return 0;
}