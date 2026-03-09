#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>
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

int isPrime(long long n)
{
    int i;

    if(n<=1) return 0;

    for(i=2;i*i<=n;i++)
        if(n%i==0)
            return 0;

    return 1;
}

int isPrimitiveRoot(long long g,long long p)
{
    int i;

    for(i=1;i<p-1;i++)
        if(modexp(g,i,p)==1)
            return 0;

    return 1;
}

int main()
{
    WSADATA wsa;
    SOCKET s,new_socket;
    struct sockaddr_in server,client;

    int c;

    long long p,g,x,h;
    long long HC1,HC2;
    long long svalue,inv,m;

    srand(time(NULL));

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

    /* INPUT FROM SERVER */

    printf("Enter prime p: ");
    scanf("%lld",&p);

    while(!isPrime(p))
    {
        printf("Enter prime again: ");
        scanf("%lld",&p);
    }

    printf("Enter primitive root g: ");
    scanf("%lld",&g);

    if(!isPrimitiveRoot(g,p))
    {
        printf("g is not primitive root\n");
        return 0;
    }

    x = rand()%(p-2)+1;

    printf("Private key x = %lld\n",x);

    h = modexp(g,x,p);

    printf("Public key h = %lld\n",h);

    /* SEND PUBLIC KEY */

    send(new_socket,(char*)&p,sizeof(p),0);
    send(new_socket,(char*)&g,sizeof(g),0);
    send(new_socket,(char*)&h,sizeof(h),0);

    /* RECEIVE HOMOMORPHIC CIPHER */

    recv(new_socket,(char*)&HC1,sizeof(HC1),0);
    recv(new_socket,(char*)&HC2,sizeof(HC2),0);

    printf("\nReceived Cipher = (%lld , %lld)\n",HC1,HC2);

    /* DECRYPTION */

    svalue = modexp(HC1,x,p);

    inv = modInverse(svalue,p);

    m = (HC2 * inv)%p;

    printf("Decrypted Value = %lld\n",m);

    closesocket(new_socket);
    WSACleanup();

    return 0;
}