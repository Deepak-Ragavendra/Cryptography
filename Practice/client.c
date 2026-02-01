#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char message[] = "Hello Server";

    // 1. Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);

    // 2. Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // 3. Server address setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 4. Connect to server
    connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 5. Send data to server
    send(client_socket, message, strlen(message), 0);

    // 6. Close socket
    closesocket(client_socket);

    // 7. Cleanup Winsock
    WSACleanup();

    return 0;
}
