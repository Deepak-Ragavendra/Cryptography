#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

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
    SOCKET socketStatus, acceptConnection;
    struct sockaddr_in serverSocket, clientSocket;
    int clientSize = sizeof(clientSocket);
    char buffer[1024];

    int k = 7;  

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("Winsock not initialized\n");
        return 0;
    }
    printf("Winsock initialized\n");

    socketStatus = socket(AF_INET, SOCK_STREAM, 0);
    if (socketStatus == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }
    printf("Socket created\n");

    serverSocket.sin_family = AF_INET;
    serverSocket.sin_addr.S_un.S_addr = inet_addr("192.168.1.7");
    serverSocket.sin_port = htons(4000);

    if (bind(socketStatus, (struct sockaddr*)&serverSocket, sizeof(serverSocket)) == SOCKET_ERROR) {
        printf("Binding failed. Error: %d\n", WSAGetLastError());
        closesocket(socketStatus);
        WSACleanup();
        return 0;
    }

    listen(socketStatus, 3);

    acceptConnection = accept(socketStatus, (struct sockaddr*)&clientSocket, &clientSize);
    if (acceptConnection == INVALID_SOCKET) {
        printf("Accept failed. Error: %d\n", WSAGetLastError());
        closesocket(socketStatus);
        WSACleanup();
        return 0;
    }

    int recvBytes = recv(acceptConnection, buffer, sizeof(buffer), 0);
    if (recvBytes == SOCKET_ERROR) {
        printf("Receive failed\n");
    } else {
        buffer[recvBytes] = '\0';
        printf("Received: %s\n", buffer);
    }

    int User_ID, n, receivedToken;
    sscanf(buffer, "%d %d %d", &User_ID, &n, &receivedToken);

    int computedToken = modExp(User_ID, k, n);

    printf("User_ID = %d\n", User_ID);
    printf("n = %d\n", n);
    printf("Received Token = %d\n", receivedToken);
    printf("Computed Token = %d\n", computedToken);

    if (computedToken == receivedToken) {
        printf("Authentication SUCCESS \n");
    } else {
        printf("Authentication FAILED \n");
    }

    closesocket(acceptConnection);
    closesocket(socketStatus);
    WSACleanup();
    return 0;
}
