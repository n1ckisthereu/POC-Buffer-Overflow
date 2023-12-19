// server.c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "../include/server.h"

static SOCKET serverSocket = INVALID_SOCKET;
static int serverPort = 65535;

DWORD WINAPI ServerThread(LPVOID lpParam) {
    // Chama a função que inicia o servidor

    StartServer();
    
    // Mantém a thread do servidor rodando indefinidamente
    while (1) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            closesocket(serverSocket);
            WSACleanup();
            return 1;  // Sair da thread em caso de erro
        }

        printf("Conexão aceita. Aguardando dados do cliente...\n");

        char buffer[1024];
        int bytesRead;

        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            // Imprime no console os dados recebidos do cliente
            buffer[bytesRead] = '\0';  // Garante que a string seja terminada
            printf("Dados recebidos do cliente: %s\n", buffer);
        }

        closesocket(clientSocket);
    }

    return 0;
}

void StartServer() {
    // Inicialize a biblioteca de soquetes do Windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Erro ao inicializar a biblioteca de soquetes\n");
        return;
    }

    // Crie um soquete para o servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        // Tratamento de erro, se a criação do soquete falhar
        fprintf(stderr, "Erro ao criar o soquete do servidor\n");
        WSACleanup();
        return;
    }

    // Configure a estrutura de informações do endereço do servidor
    struct sockaddr_in serverAddr; // Adicione o 'struct' aqui
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    // Associe o soquete ao endereço e porta do servidor
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        // Tratamento de erro, se a associação falhar
        fprintf(stderr, "Erro ao associar soquete ao endereço e porta do servidor\n")
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Comece a escutar por conexões
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        // Tratamento de erro, se a escuta falhar
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    fflush(stdin);
    fprintf(stderr, "On - Server started port %i\n", serverPort);
}

void StopServer() {
    if (serverSocket != INVALID_SOCKET) {
        // Feche o soquete do servidor
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    // Limpe a biblioteca de soquetes do Windows
    WSACleanup();

    printf("Off - Connection has as closed \n");
}
