#include <stdio.h>
#include <Winsock2.h>
#pragma comment (lib,"Ws2_32.lib")

int modExp(int base, int exp, int mod) {
    int result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        exp = exp / 2;
        base = (base * base) % mod;
    }
    return result;
}

int main() {
    WSADATA wsadata;
    SOCKET socketstatus;
    struct sockaddr_in clientSocket;

    int User_ID;
    printf("Enter User ID: ");
    scanf("%d", &User_ID);

    int k = 7;   
    int n = 9;   

    int Token = modExp(User_ID, k, n);

    char buffer[100];
    sprintf(buffer, "%d %d %d", User_ID, n, Token);

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    socketstatus = socket(AF_INET, SOCK_STREAM, 0);
    if (socketstatus == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    clientSocket.sin_family = AF_INET;
    clientSocket.sin_addr.S_un.S_addr = inet_addr("192.168.1.7");
    clientSocket.sin_port = htons(4000);

    if (connect(socketstatus, (struct sockaddr*)&clientSocket, sizeof(clientSocket)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(socketstatus);
        WSACleanup();
        return 1;
    }

    printf("Connected to server\n");
    printf("Sending: User_ID=%d  n=%d  Token=%d\n", User_ID, n, Token);

    send(socketstatus, buffer, strlen(buffer), 0);

    closesocket(socketstatus);
    WSACleanup();
    return 0;
}
