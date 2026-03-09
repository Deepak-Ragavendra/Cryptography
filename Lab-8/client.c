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

int main()
{
    srand(time(NULL));

    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    long long p,g,h;
    long long m1,m2;
    long long r1,r2;

    long long C11,C21,C12,C22;
    long long HC1,HC2;

    WSAStartup(MAKEWORD(2,2),&wsa);

    s = socket(AF_INET , SOCK_STREAM , 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8888);

    connect(s,(struct sockaddr *)&server,sizeof(server));

    /* RECEIVE PUBLIC KEY */

    recv(s,(char*)&p,sizeof(p),0);
    recv(s,(char*)&g,sizeof(g),0);
    recv(s,(char*)&h,sizeof(h),0);

    printf("Received Public Key:\n");
    printf("p = %lld\n",p);
    printf("g = %lld\n",g);
    printf("h = %lld\n",h);

    printf("Enter message1: ");
    scanf("%lld",&m1);

    printf("Enter message2: ");
    scanf("%lld",&m2);

    r1 = rand()%(p-2)+1;
    r2 = rand()%(p-2)+1;

    printf("Nonce r1 = %lld\n",r1);
    printf("Nonce r2 = %lld\n",r2);

    /* ENCRYPTION */

    C11 = modexp(g,r1,p);
    C21 = (m1 * modexp(h,r1,p)) % p;

    C12 = modexp(g,r2,p);
    C22 = (m2 * modexp(h,r2,p)) % p;

    printf("\nE(m1) = (%lld , %lld)\n",C11,C21);
    printf("E(m2) = (%lld , %lld)\n",C12,C22);

    /* HOMOMORPHIC MULTIPLICATION */

    HC1 = (C11*C12)%p;
    HC2 = (C21*C22)%p;

    printf("\nHomomorphic Cipher:\n");
    printf("E(m1)*E(m2) = (%lld , %lld)\n",HC1,HC2);

    printf("Expected Result (m1*m2 mod p) = %lld\n",(m1*m2)%p);

    /* SEND TO SERVER */

    send(s,(char*)&HC1,sizeof(HC1),0);
    send(s,(char*)&HC2,sizeof(HC2),0);

    closesocket(s);
    WSACleanup();

    return 0;
}