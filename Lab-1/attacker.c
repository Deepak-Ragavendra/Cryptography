#include<stdio.h>
#include<string.h>
#include<time.h>
#include<math.h>
int main(){
    clock_t start, end;
    double time_taken;
    const char *mgs="Fkdudfwhu jurzwk lv d glu hf wuhvxow ri Jr'g wudqvirupdwlyh zrun, jxlglqj rxu prudo ghflvlrqv";
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
    end = clock();
    time_taken = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    printf("Time taken: %.3f ms\n", time_taken);
    printf("Number of Characters: %d\n", len);
    return 0;
}