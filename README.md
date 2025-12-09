README - Progetto Reti di Calcolatori
AA 2025/2026  
Gruppo G54  
Autori: Daniele Sardone, Ferdinando Marchetti  

------------------------------------------------------------
AMBIENTE DI SVILUPPO
------------------------------------------------------------
• Sistema Operativo: Windows 11  / Linux
• IDE utilizzati: Eclipse CDT  
• Linguaggio: C  

------------------------------------------------------------
NOTE TECNICHE (IMPORTANTE)
------------------------------------------------------------
Per la compilazione dei programmi Client/Server su Windows
è necessario aggiungere il parametro linker:

    -lws2_32

Questo vale sia per i programmi TCP che UDP.

------------------------------------------------------------
DESCRIZIONE DEL PROGETTO
------------------------------------------------------------

Il progetto include due applicazioni distinte:

1) **Applicazione TCP (client/server)**
   - Il client richiede il nome del server e stabilisce la connessione.
   - Dopo la connessione riceve il messaggio “connessione avvenuta”.
   - Invia una lettera (A/S/M/D) per scegliere l’operazione.
   - Se l’operazione è valida, invia due numeri interi.
   - Riceve il risultato dal server e termina.

   Il server:
   - Rimane sempre in ascolto sulla porta 27015.
   - Gestisce un client alla volta.
   - Processa l’operazione richiesta e invia il risultato.

2) **Applicazione UDP (client/server)**
   - Stesso protocollo logico dell’applicazione TCP,
     eccetto che manca la fase di connessione.
   - Comunicazione basata su messaggi UDP.

------------------------------------------------------------
STRUTTURA DEI FILE
------------------------------------------------------------
• client_tcp_g54.c  
• server_tcp_g54.c  
• client_udp_g54.c  
• server_udp_g54.c  

------------------------------------------------------------
COMPILAZIONE (Esempi)
------------------------------------------------------------

GCC / MinGW:

    gcc client_tcp_g54.c -o client_tcp_g54.exe -lws2_32
    gcc server_tcp_g54.c -o server_tcp_g54.exe -lws2_32

    gcc client_udp_g54.c -o client_udp_g54.exe -lws2_32
    gcc server_udp_g54.c -o server_udp_g54.exe -lws2_32

------------------------------------------------------------
ESECUZIONE
------------------------------------------------------------

1) Avviare sempre prima il server:
       server_tcp_g54.exe
       server_udp_g54.exe

2) Poi avviare il client:
       client_tcp_g54.exe
       client_udp_g54.exe

------------------------------------------------------------
FINE DOCUMENTO
------------------------------------------------------------
