#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <time.h>
#pragma comment(lib,"Ws2_32.lib")

int isPrime(int n)
{
    int i;
    if(n<=1) return 0;

    for(i=2;i*i<=n;i++)
        if(n%i==0)
            return 0;

    return 1;
}

int power(int x,int y,int p)
{
    int res=1;

    while(y>0)
    {
        if(y%2==1)
            res=(res*x)%p;

        x=(x*x)%p;
        y/=2;
    }

    return res;
}

int main()
{
    srand(time(NULL));
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    int p;
    int secret;
    int k,n;

    int a[10];
    int x[10],y[10];

    int i,j;

    printf("Enter prime p: ");
    scanf("%d",&p);

    while(!isPrime(p))
    {
        printf("Enter prime again: ");
        scanf("%d",&p);
    }

    secret = rand() % (p-1) + 1;
    k=0;
    while(k<1){
        printf("Enter k (threshold): ");
        scanf("%d",&k);
    }

    printf("Enter n (shares): ");
    scanf("%d",&n);

    a[0]=secret;

    /* Random coefficients */

    for(i=1;i<k;i++)
        a[i]=rand()%10;

    printf("\nPolynomial:\n");

    for(i=0;i<k;i++)
        printf("a%d = %d\n",i,a[i]);
    
    /* Generate shares */

    printf("\nGenerated Shares:\n");

    for(i=1;i<=n;i++)
    {
        x[i]=i;
        y[i]=0;

        for(j=0;j<k;j++)
        {
            int val = power(x[i],j,p);
            y[i]=(y[i]+a[j]*val)%p;
        }

        printf("(%d , %d)\n",x[i],y[i]);
    }

    /* Socket */

    WSAStartup(MAKEWORD(2,2),&wsa);

    s = socket(AF_INET , SOCK_STREAM , 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8888);

    connect(s,(struct sockaddr *)&server,sizeof(server));

    /* Send p and k */

    send(s,(char*)&p,sizeof(p),0);
    send(s,(char*)&k,sizeof(k),0);

    printf("\nSending first %d shares to server\n",k);

    printf("Secret Key: %d\n",secret);

    for(i=1;i<=k;i++)
    {
        send(s,(char*)&x[i],sizeof(int),0);
        send(s,(char*)&y[i],sizeof(int),0);
    }

    closesocket(s);
    WSACleanup();

    return 0;
}