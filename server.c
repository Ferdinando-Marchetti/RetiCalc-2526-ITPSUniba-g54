#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PROTOPORT 27015
#define QLEN 6
#define BUFFERSIZE 512

void ErrorHandler(char *errorMessage) { printf("%s", errorMessage); }

void ClearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}

int main() {

#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        ErrorHandler("WSAStartup failed.\n");
        return 0;
    }
#endif

    int welcomeSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (welcomeSocket < 0) {
        ErrorHandler("Socket creation failed.\n");
        ClearWinSock();
        return -1;
    }

    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = INADDR_ANY;
    sad.sin_port = htons(PROTOPORT);

    if (bind(welcomeSocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        ErrorHandler("Bind failed.\n");
        closesocket(welcomeSocket);
        ClearWinSock();
        return -1;
    }

    if (listen(welcomeSocket, QLEN) < 0) {
        ErrorHandler("Listen failed.\n");
        closesocket(welcomeSocket);
        ClearWinSock();
        return -1;
    }

    printf("SERVER IN ASCOLTO sulla porta %d...\n", PROTOPORT);

    while (1) {
        struct sockaddr_in cad;
        int clientLen = sizeof(cad);

        int clientSocket = accept(welcomeSocket, (struct sockaddr*)&cad, &clientLen);
        if (clientSocket < 0) {
            ErrorHandler("Accept failed.\n");
            continue;
        }

        printf("Client connesso: %s\n", inet_ntoa(cad.sin_addr));

        // 4) invio stringa connessione
        send(clientSocket, "connessione avvenuta", 21, 0);

        // 7) ricezione lettera
        char lettera[4];
        int bytesRcvd = recv(clientSocket, lettera, sizeof(lettera)-1, 0);
        if (bytesRcvd <= 0) {
            closesocket(clientSocket);
            continue;
        }
        lettera[bytesRcvd] = '\0';

        char operazione[BUFFERSIZE];

        if (lettera[0] == 'A' || lettera[0] == 'a')
            strcpy(operazione, "ADDIZIONE");
        else if (lettera[0] == 'S' || lettera[0] == 's')
            strcpy(operazione, "SOTTRAZIONE");
        else if (lettera[0] == 'M' || lettera[0] == 'm')
            strcpy(operazione, "MOLTIPLICAZIONE");
        else if (lettera[0] == 'D' || lettera[0] == 'd')
            strcpy(operazione, "DIVISIONE");
        else {
            strcpy(operazione, "TERMINE PROCESSO CLIENT");
            send(clientSocket, operazione, strlen(operazione), 0);
            closesocket(clientSocket);
            continue;
        }

        send(clientSocket, operazione, strlen(operazione), 0);

        // 9) ricezione due interi
        int n1, n2;
        recv(clientSocket, (char*)&n1, sizeof(int), 0);
        recv(clientSocket, (char*)&n2, sizeof(int), 0);

        int risultato;

        if (!strcmp(operazione, "ADDIZIONE"))
            risultato = n1 + n2;
        else if (!strcmp(operazione, "SOTTRAZIONE"))
            risultato = n1 - n2;
        else if (!strcmp(operazione, "MOLTIPLICAZIONE"))
            risultato = n1 * n2;
        else
            risultato = n1 / n2;

        send(clientSocket, (char*)&risultato, sizeof(int), 0);

        closesocket(clientSocket);
    }

    closesocket(welcomeSocket);
    ClearWinSock();
    return 0;
}