#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

int isPrime(int n)
{
    int i;

    if(n<=1) return 0;

    for(i=2;i*i<=n;i++)
        if(n%i==0)
            return 0;

    return 1;
}

int isPrimitiveRoot(int g,int p)
{
    int i;

    for(i=1;i<p-1;i++)
        if(modexp(g,i,p)==1)
            return 0;

    return 1;
}

int main()
{
    srand(time(NULL)); 
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    long long p,g,x,h;
    long long m1,m2;
    long long r1,r2;

    long long C11,C21,C12,C22;
    long long HC1,HC2;

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

    printf("Enter message1: ");
    scanf("%lld",&m1);

    printf("Enter message2: ");
    scanf("%lld",&m2);

    r1 = rand()%5 + 1;
    r2 = rand()%5 + 1;

    printf("Nonce r1 = %lld\n",r1);
    printf("Nonce r2 = %lld\n",r2);

    /* Encryption */

    C11 = modexp(g,r1,p);
    C21 = (m1 * modexp(h,r1,p)) % p;

    C12 = modexp(g,r2,p);
    C22 = (m2 * modexp(h,r2,p)) % p;

    printf("\nE(m1) = (%lld , %lld)\n",C11,C21);
    printf("E(m2) = (%lld , %lld)\n",C12,C22);

    /* Homomorphic Multiplication */

    HC1 = (C11*C12)%p;
    HC2 = (C21*C22)%p;

    printf("\nHomomorphic Encryption:\n");
    printf("E(m1)*E(m2) = (%lld , %lld)\n",HC1,HC2);

    /* Socket */

    WSAStartup(MAKEWORD(2,2),&wsa);

    s = socket(AF_INET , SOCK_STREAM , 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8888);

    connect(s,(struct sockaddr *)&server,sizeof(server));

    send(s,(char*)&HC1,sizeof(HC1),0);
    send(s,(char*)&HC2,sizeof(HC2),0);

    send(s,(char*)&x,sizeof(x),0);
    send(s,(char*)&p,sizeof(p),0);

    closesocket(s);
    WSACleanup();

    return 0;
}