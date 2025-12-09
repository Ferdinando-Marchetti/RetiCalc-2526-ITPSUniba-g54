#if defined WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROTOPORT 27015
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
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandler("WSAStartup failed.\n");
        return 0;
    }
#endif

    int serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket < 0) {
        ErrorHandler("Socket creation failed.\n");
        ClearWinSock();
        return -1;
    }

    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = INADDR_ANY;
    sad.sin_port = htons(PROTOPORT);

    if (bind(serverSocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        ErrorHandler("Bind failed.\n");
        closesocket(serverSocket);
        ClearWinSock();
        return -1;
    }

    printf("SERVER UDP in ascolto sulla porta %d...\n", PROTOPORT);

    while (1) {

        struct sockaddr_in cad;
        socklen_t clientLen = sizeof(cad);
        char lettera[4];

        // 1) ricezione lettera
        int bytesRcvd = recvfrom(serverSocket, lettera, sizeof(lettera)-1, 0,
                                 (struct sockaddr*)&cad, &clientLen);
        if (bytesRcvd <= 0) continue;

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
            sendto(serverSocket, operazione, strlen(operazione), 0,
                   (struct sockaddr*)&cad, clientLen);
            continue;
        }

        // 2) invio operazione
        sendto(serverSocket, operazione, strlen(operazione), 0,
               (struct sockaddr*)&cad, clientLen);

        // 3) ricezione numeri
        int n1, n2;
        recvfrom(serverSocket, (char*)&n1, sizeof(int), 0,
                 (struct sockaddr*)&cad, &clientLen);
        recvfrom(serverSocket, (char*)&n2, sizeof(int), 0,
                 (struct sockaddr*)&cad, &clientLen);

        int risultato;
        if (!strcmp(operazione, "ADDIZIONE")) risultato = n1 + n2;
        else if (!strcmp(operazione, "SOTTRAZIONE")) risultato = n1 - n2;
        else if (!strcmp(operazione, "MOLTIPLICAZIONE")) risultato = n1 * n2;
        else risultato = (n2 == 0 ? 0 : n1 / n2);

        // 4) invio risultato
        sendto(serverSocket, (char*)&risultato, sizeof(int), 0,
               (struct sockaddr*)&cad, clientLen);
    }

    closesocket(serverSocket);
    ClearWinSock();
    return 0;
}
