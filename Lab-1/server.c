#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib,"Ws2_32.lib")
int main() {
    WSADATA wsadata;
    SOCKET socketStatus;
    struct sockaddr_in structsocket, clientSocket;
    int clientSze = sizeof(clientSocket);
    char buffer[1024];
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        printf("Winsock in not initialized\n");
        return 0;
    }
    else {
        printf("Winsock in initialized\n");
    }
    socketStatus = socket(AF_INET, SOCK_STREAM, 0);


    if (socketStatus == INVALID_SOCKET) {
        printf("Socket failed to create");
        return 1;
    }
    printf("socket created\n");
    structsocket.sin_family = AF_INET;
    structsocket.sin_addr.S_un.S_addr = inet_addr("192.168.1.6");
    structsocket.sin_port = htons(4000);


    int bindingStatus = bind(socketStatus, (struct sockaddr*)&structsocket, sizeof(struct sockaddr_in));
    if (bindingStatus == SOCKET_ERROR) {
        printf("Binding failed Error Code : %d\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }


    listen(socketStatus, 3);
   
    int acceptConnection = accept(socketStatus, (struct sockaddr*)&clientSocket, &clientSze);
    if (acceptConnection == INVALID_SOCKET) {
        printf("Connection is not estalished with client. Error Code : %d\n", WSAGetLastError());
        closesocket(socketStatus);
        WSACleanup();
        return 0;
    }
    printf("Connection Accepted to client\n");
   
    int receiveConnectionfromClient = recv(acceptConnection, buffer, sizeof(buffer), 0);
    if (receiveConnectionfromClient == SOCKET_ERROR) {
        printf("Recv failed. Error Code: %d\n", WSAGetLastError());
        closesocket(socketStatus);
        WSACleanup();
    }
    else {
        buffer[receiveConnectionfromClient] = '\0';
        printf("Received: %s\n", buffer);
    }
    int len=strlen(buffer);
    char final[100];
    for (int i = 0; i < len; i++) {
        char c = buffer[i];


        if (c >= 'A' && c <= 'Z') {
            final[i] = (char)((c - 'A' - 3) % 26 + 'A');
        }
        else if (c >= 'a' && c <= 'z') {
            final[i] = (char)((c - 'a' - 3) % 26 + 'a');
        }
        else {
            final[i] = c; 
        }
    }


    final[len] = '\0';
    printf("Decoded : %s\n", final);


        closesocket(receiveConnectionfromClient);
        closesocket(socketStatus);
        WSACleanup();
    return 0;


}
