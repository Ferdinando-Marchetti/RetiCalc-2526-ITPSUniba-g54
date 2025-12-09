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

    // 1) Richiesta hostname
    char serverName[100];
    printf("Inserisci nome server (es. localhost): ");
    scanf("%99s", serverName);

    // 2) Risoluzione hostname
    struct hostent *host;
    host = gethostbyname(serverName);
    if (!host) {
        ErrorHandler("Errore risoluzione hostname.\n");
        ClearWinSock();
        return -1;
    }

    // 3) Conversione IP leggibile
    struct in_addr addr;
    memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));
    char *serverIP = inet_ntoa(addr);
    printf("IP del server risolto: %s\n", serverIP);

    // 4) Creazione socket
    int Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Csocket < 0) {
        ErrorHandler("socket() failed.\n");
        return -1;
    }

    // 5) Configurazione indirizzo server
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    memcpy(&sad.sin_addr, host->h_addr_list[0], host->h_length);
    sad.sin_port = htons(PROTOPORT);

    // 6) Connessione
    if (connect(Csocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        ErrorHandler("Connect failed.\n");
        closesocket(Csocket);
        ClearWinSock();
        return -1;
    }

    // 7) Ricezione messaggio iniziale
    char buf[BUFFERSIZE];
    int bytesRcvd = recv(Csocket, buf, BUFFERSIZE - 1, 0);
    if (bytesRcvd <= 0) {
        ErrorHandler("Errore ricezione dal server.\n");
        closesocket(Csocket);
        ClearWinSock();
        return -1;
    }
    buf[bytesRcvd] = '\0';
    printf("SERVER: %s\n", buf);

    // 8) Invio lettera
    char lettera;
    printf("Inserisci lettera (A/S/M/D): ");
    scanf(" %c", &lettera);
    send(Csocket, &lettera, 1, 0);

    // 9) Ricezione istruzioni
    bytesRcvd = recv(Csocket, buf, BUFFERSIZE - 1, 0);
    if (bytesRcvd <= 0) {
        ErrorHandler("Errore ricezione dal server.\n");
        closesocket(Csocket);
        ClearWinSock();
        return -1;
    }
    buf[bytesRcvd] = '\0';
    printf("SERVER: %s\n", buf);

    if (!strcmp(buf, "TERMINE PROCESSO CLIENT")) {
        closesocket(Csocket);
        ClearWinSock();
        return 0;
    }

    // 10) Invio numeri
    int n1, n2;
    printf("Inserisci primo numero: ");
    scanf("%d", &n1);
    printf("Inserisci secondo numero: ");
    scanf("%d", &n2);

    send(Csocket, (char*)&n1, sizeof(int), 0);
    send(Csocket, (char*)&n2, sizeof(int), 0);

    // 11) Ricezione risultato
    int risultato;
    recv(Csocket, (char*)&risultato, sizeof(int), 0);

    printf("RISULTATO = %d\n", risultato);

    // 12) Chiusura
    closesocket(Csocket);
    ClearWinSock();

    return 0;
}
