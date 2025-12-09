#if defined WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 512
#define PROTOPORT 27015

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

    char serverName[100];
    printf("Inserisci nome server (es. localhost): ");
    scanf("%99s", serverName);

    struct hostent *host = gethostbyname(serverName);
    if (!host) {
        ErrorHandler("Errore risoluzione hostname.\n");
        ClearWinSock();
        return -1;
    }

    struct in_addr addr;
    memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));
    char *serverIP = inet_ntoa(addr);
    printf("IP del server risolto: %s\n", serverIP);

    int Csocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (Csocket < 0) {
        ErrorHandler("socket() failed.\n");
        return -1;
    }

    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    memcpy(&sad.sin_addr, host->h_addr_list[0], host->h_length);
    sad.sin_port = htons(PROTOPORT);

    // 1) Invio lettera al server
    char lettera;
    printf("Inserisci lettera (A/S/M/D): ");
    scanf(" %c", &lettera);

    sendto(Csocket, &lettera, 1, 0, (struct sockaddr*)&sad, sizeof(sad));

    // 2) Ricezione risposta del server
    char buf[BUFFERSIZE];
    socklen_t addrlen = sizeof(sad);
    int bytesRcvd = recvfrom(Csocket, buf, BUFFERSIZE - 1, 0,
                             (struct sockaddr*)&sad, &addrlen);
    buf[bytesRcvd] = '\0';
    printf("SERVER: %s\n", buf);

    // se non è valida
    if (!strcmp(buf, "TERMINE PROCESSO CLIENT")) {
        closesocket(Csocket);
        ClearWinSock();
        return 0;
    }

    // 3) Invio dei due numeri
    int n1, n2;
    printf("Inserisci primo numero: ");
    scanf("%d", &n1);
    printf("Inserisci secondo numero: ");
    scanf("%d", &n2);

    sendto(Csocket, (char*)&n1, sizeof(int), 0, (struct sockaddr*)&sad, sizeof(sad));
    sendto(Csocket, (char*)&n2, sizeof(int), 0, (struct sockaddr*)&sad, sizeof(sad));

    // 4) Ricezione risultato
    int risultato;
    recvfrom(Csocket, (char*)&risultato, sizeof(int), 0,
             (struct sockaddr*)&sad, &addrlen);

    printf("RISULTATO = %d\n", risultato);

    closesocket(Csocket);
    ClearWinSock();
    return 0;
}
