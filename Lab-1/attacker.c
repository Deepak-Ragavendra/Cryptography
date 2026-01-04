#include<stdio.h>
#include<string.h>
#include<math.h>
int main(){
    const char *mgs="Kl iurp folhqw";
    int len=strlen(mgs);
    for(int i=1;i<=26;i++){
        printf("%d.",i);
        for(int j=0;j<len;j++){
            char a=mgs[j];
            if(a>='A' && a<='Z'){
                char b=(char)(abs(a-'A'-i)%26+'A');
                printf("%c",b);
            }
            else if(a>='a' && a<='z'){
                char b=(char)(abs(a-'a'-i)%26+'a');
                printf("%c",b);
            }
            else{
                printf("%c",a);
            }
        }
        printf("\n");
    }
    return 0;
}