
#include <iostream>
#include <sys/locking.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <string.h>
#include <time.h>

#pragma comment (lib, "ws2_32.lib") //libreria winsock
#pragma warning (disable : 4996);

#define STANDARD_STRING_LENGTH 80

struct sockaddr_in myaddr_in;
struct sockaddr_in peeraddr_in;
struct hostent* hp;

void stampaTabella();

int main(int argc, char* argv[]) {

	WSADATA wsa;
	SOCKET mySock;
	printf("Inizializzo Winsock lato client\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {//inizializzazione socket
		printf("Errore\n");
		return 1;
	}
	else printf("Inizializzato\n");
	if ((mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {//creo socket
		printf("Impossibile creare il socket: %d\n", WSAGetLastError());
		return 1;
	}
	else printf("Socket creato\n");
	myaddr_in.sin_family = AF_INET;
	hp = gethostbyname((char*)argv[1]);
	//myaddr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
	myaddr_in.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;//Assegno indirizzo IP
	myaddr_in.sin_port = htons(10000); //Assegno indirizzo porta
	printf("Porta: %d\n", ntohs(myaddr_in.sin_port));

	int structSize = sizeof(myaddr_in);

	if (connect(mySock, (struct sockaddr*)&myaddr_in, sizeof(myaddr_in)) < 0) {//tento connessione
		printf("Errore connect\n");
		return 1;
	}
	else {

		char saluto[STANDARD_STRING_LENGTH];
		char msg[STANDARD_STRING_LENGTH];

		do {
			// stampo la tabella dei saluti
			stampaTabella();

			// leggo il saluto dell'utente
			fgets(saluto, 80, stdin);
			// elimino il '\n'
			saluto[strlen(saluto)-1] = '\0';

			printf("\n\n");

			// invio il saluto al server
			sendto(mySock, saluto, sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&myaddr_in, sizeof(myaddr_in));

			// ricevo un messaggio dal server
			recvfrom(mySock, msg, sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&peeraddr_in, &structSize);

			// stampo il messaggio del server
			printf("Messaggio del server: %s\n\n", msg);
			system("PAUSE");
			system("CLS");
		} while (1);
	}


	system("PAUSE");
}

// la funzione stampaTabella stampa la tabella dei saluti in funzione degli orari
void stampaTabella() {
	printf("Tabella dei saluti corretti:\n");
	printf("\tFino alle 13 (comprese) -> Buongiorno\n");
	printf("\tDalle 13 alle 17 -> Buon pomeriggio\n");
	printf("\tDalle 17 (comprese) in poi -> Buonasera\n\n");
	printf("Inserisci un saluto: ");
}
