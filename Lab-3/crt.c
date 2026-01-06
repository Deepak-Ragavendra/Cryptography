#include<stdio.h>

int module(int a,int b){
    if(b==0){
        return a;
    }
    return module(b,a%b);
}

int find(int N,int n){
    int i=1;
    while(i>=0){
        if((N*i)%n==1){
            break;
        }
        else{
            i++;
        }
    }
    return i;
}

int calculate_crt(int a[],int m[],int M,int n){
    int N[n],x=0,k;
    for(int i=0;i<n;i++){
        N[i]=M/m[i];
    }
    int inv[n];
    for(int i=0;i<n;i++){
        inv[i]=find(N[i],m[i]);
    }
    for(int i=0;i<n;i++){
        x=x+(a[i]*N[i]*inv[i]);
    }
    return x%M;
}

int main(){
    int n;
    printf("Enter the number of remainders and moduli: ");
    scanf("%d",&n);
    int a[n],m[n],M=1;
    int k=0;
    int gcd=0;
    for(int i=0;i<n;i++){
        printf("Enter the remainder :");
        scanf("%d",&a[i]);
        printf("Enter the modulus :");
        scanf("%d",&m[i]);
        M=M*m[i];
        if(i==0){
            gcd=m[0];
        }
        else{
            gcd=module(gcd,m[i]);
        }
    }
    if(gcd==1){
        int x=calculate_crt(a,m,M,n);
        printf("The valid number would be %d",x);
    }
    else{
        printf("The n's are not co-prime ,CRT will not work!!");
    }

}