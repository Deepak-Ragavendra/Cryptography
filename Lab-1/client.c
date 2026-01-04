#include <stdio.h>
#include <string.h>    
#include <Winsock2.h> 
#pragma comment (lib,"Ws2_32.lib")


int main() {
    WSADATA wsadata;
    SOCKET socketstatus;
    const char* msg = "Hi from client";
    struct sockaddr_in clientSocket;
    int len = strlen(msg);
   
    char token[100]; 
    if (len >= 100) {
        printf("Error: message is too long for the buffer.\n");
        return 1;
    }



    int initWSA = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (initWSA != 0) {
        printf("WSA failed to initialize %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }



    socketstatus = socket(AF_INET, SOCK_STREAM, 0);
    if (socketstatus == INVALID_SOCKET) {
        printf("Failed in initializing the socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
   
 
    clientSocket.sin_family = AF_INET;
    clientSocket.sin_addr.S_un.S_addr = inet_addr("192.168.1.6");
    clientSocket.sin_port = htons(4000);


    int clientConnection = connect(socketstatus, (struct sockaddr*)&clientSocket, sizeof(clientSocket));
    if (clientConnection == SOCKET_ERROR) {
        printf("Failed to connect to the server at %s:%d. Error Code %d\n",
               inet_ntoa(clientSocket.sin_addr), ntohs(clientSocket.sin_port), WSAGetLastError());
        closesocket(socketstatus);
        WSACleanup();
        return 1;
    }
    printf("Client is connected with server\n");


    for (int i = 0; i < len; i++) {
        char c = msg[i];


        if (c >= 'A' && c <= 'Z') {
            token[i] = (char)((c - 'A' + 3) % 26 + 'A');
        }
        else if (c >= 'a' && c <= 'z') {
            token[i] = (char)((c - 'a' + 3) % 26 + 'a');
        }
        else {
            token[i] = c; 
        }
    }


    token[len] = '\0'; 
    printf("Encoded : %s\n", token);


    int bytes_to_send = strlen(token);
    int sendData = send(socketstatus, token, bytes_to_send, 0);
   
    if (sendData == SOCKET_ERROR) {
        printf("Failed to send the data. Error code: %d\n", WSAGetLastError());
        closesocket(socketstatus);
        WSACleanup();
        return 1;
    } else if (sendData != bytes_to_send) {
        printf("Warning: Only sent %d out of %d bytes.\n", sendData, bytes_to_send);
    } else {
        printf("Successfully sent %d bytes of data.\n", sendData);
    }
   
    closesocket(socketstatus);
    WSACleanup();
   
    return 0;
}
