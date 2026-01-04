#include <stdio.h>

int main() {
    int user_id = 2;   // from Wireshark
    int token   = 2;   // from Wireshark

    printf("Replaying captured credentials...\n");
    printf("User_ID = %d\n", user_id);
    printf("Token   = %d\n", token);

    printf("\nServer will ACCEPT\n");
    return 0;
}
