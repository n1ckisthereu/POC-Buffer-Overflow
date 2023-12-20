// server.c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "../include/server.h"

static SOCKET serverSocket = INVALID_SOCKET;
static int serverPort = 65535;

void copy(char str[8000]) { // Vulnerable function
    char cpy[84];
    strcpy(cpy, str); // Vulnerable section
}

void StartServer() {
    // Initialize the Windows Socket Library
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Error initializing socket library\n");
        return;
    }

    // Create a socket for the server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        // Error handling if socket creation fails
        fprintf(stderr, "Error creating server socket\n");
        WSACleanup();
        return;
    }

    // Configure the server address information structure
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    // Associate the socket with the server address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        // Error handling if the association fails
        fprintf(stderr, "Error associating socket with server address and port\n");
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Start listening for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        // Error handling if listening fails
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    fprintf(stderr, "On - Server started port %i\n", serverPort);

    // Accept and process connections indefinitely
    while (1) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            fprintf(stderr, "Error accepting connection\n");
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        printf("Connection accepted. Waiting for customer details...\n");

        char buffer[1024];
        int bytesRead;

        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            // Prints data received from the client to the console
            buffer[bytesRead] = '\0';  // Ensures the string is terminated
            printf("Data received from the customer: %s\n", buffer);

            // Call the vulnerable copy function
            copy(buffer);

            // Continue processing as needed

            // Example: send a response back
            send(clientSocket, "Message received", sizeof("Message received"), 0);
        }

        closesocket(clientSocket);
    }
}

void StopServer() {
    if (serverSocket != INVALID_SOCKET) {
        // Close the server socket
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    // Clean the Windows Socket Library
    WSACleanup();
    printf("Off - Connection has as closed \n");
}

DWORD WINAPI ServerThread(LPVOID lpParam) {
    // Start the server in the thread
    StartServer();

    // Add any additional logic for your ServerThread as needed

    return 0;
}