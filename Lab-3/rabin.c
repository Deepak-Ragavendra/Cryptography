#include<stdio.h>
#include<stdlib.h>
#include<time.h>

long long power(long long a,long long d,long long n){
    long long result=1;
    a=a%n;
    while(d>0){
        if(d%2==1){
            result=(result*a)%n;
        }
        a=(a*a)%n;
        d=d/2;
    }

    return result;
}

int millerTest(long long d,long long n){
    long long a=2+rand()%(n-4);
    long long x=power(a,d,n);
    if(x==1||x==n-1){
        return 1;
    }
    while(d!=n-1){
        x=(x*x)%n;
        d=d*2;

        if(x==1){
            return 0;
        }

        if(x==n-1){
            return 1;
        }
    }
    return 0;
}

int isPrime(long long n,int k){
    if(n<=1||n==4){
        return 0;
    }
    if(n<=3){
        return 1;
    }
    long long d=n-1;
    while(d%2==0){
        d=d/2;
    }
    for(int i=0;i<k;i++){
        if(millerTest(d,n)==0){
            return 0;
        }
    }
    return 1;
}

int main(){
    long long n;
    int k=5;
    scanf("%lld",&n);
    srand(time(NULL));
    if(isPrime(n,k)){
        printf("%lld is prime\n",n);
    }
    else{
        printf("%lld is not prime\n",n);
    }
    return 0;
}
