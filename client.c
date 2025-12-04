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
#include <netdb.h>

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
    scanf("%s", serverName);

    struct hostent *host;
    host = gethostbyname(serverName);
    if (!host) {
        ErrorHandler("Errore risoluzione hostname.\n");
        ClearWinSock();
        return -1;
    }

    char *serverIP = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);

    int Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Csocket < 0) {
        ErrorHandler("socket() failed");
        return -1;
    }

    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    memcpy(&sad.sin_addr, host->h_addr_list[0], host->h_length);
    sad.sin_port = htons(PROTOPORT);

    if (connect(Csocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        ErrorHandler("Connect failed.\n");
        closesocket(Csocket);
        ClearWinSock();
        return -1;
    }

    // 5) ricezione connessione avvenuta
    char buf[BUFFERSIZE];
    int bytesRcvd = recv(Csocket, buf, BUFFERSIZE-1, 0);
    buf[bytesRcvd] = '\0';
    printf("SERVER: %s\n", buf);

    // 6) invio lettera
    char lettera;
    printf("Inserisci lettera (A/S/M/D): ");
    scanf(" %c", &lettera);
    send(Csocket, &lettera, 1, 0);

    // 8) ricezione operazione
    bytesRcvd = recv(Csocket, buf, BUFFERSIZE-1, 0);
    buf[bytesRcvd] = '\0';
    printf("SERVER: %s\n", buf);

    if (!strcmp(buf, "TERMINE PROCESSO CLIENT")) {
        closesocket(Csocket);
        ClearWinSock();
        return 0;
    }

    int n1, n2;
    printf("Inserisci primo numero: ");
    scanf("%d", &n1);
    printf("Inserisci secondo numero: ");
    scanf("%d", &n2);

    send(Csocket, (char*)&n1, sizeof(int), 0);
    send(Csocket, (char*)&n2, sizeof(int), 0);

    int risultato;
    recv(Csocket, (char*)&risultato, sizeof(int), 0);

    printf("RISULTATO = %d\n", risultato);

    closesocket(Csocket);
    ClearWinSock();
    return 0;
}