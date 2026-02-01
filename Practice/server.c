#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // 1. Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);

    // 2. Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // 3. Server address setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 4. Bind socket
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // 5. Listen for client
    listen(server_socket, 1);
    printf("Server waiting for client...\n");

    // 6. Accept client connection
    client_socket = accept(server_socket, NULL, NULL);
    printf("Client connected!\n");

    // 7. Receive data from client
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Received from client: %s\n", buffer);

    // 8. Close sockets
    closesocket(client_socket);
    closesocket(server_socket);

    // 9. Cleanup Winsock
    WSACleanup();

    return 0;
}
