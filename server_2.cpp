
#include <iostream>
#include <sys/locking.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#include <time.h>
#include <string.h>

#pragma comment (lib, "Ws2_32.lib") //libreria winsock
#pragma warning (disable : 4996);

#define STANDARD_STRING_LENGTH 80

struct sockaddr_in myaddr_in;
struct sockaddr_in peeraddr_in;

const char* const saluti[]{ "Buongiorno","Buon pomeriggio","Buonasera","#" };
int getSaluto(char*);
int salutoCorretto();

int main() {
	SOCKET mySock, remoteSocket;
	WSADATA wsa;
	DWORD dwThreadId;
	printf("Inizializzo Winsock lato server\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {//inizializzo socket
		printf("\nErrore");
		return 1;
	}
	printf("Inizializzato\n");
	if ((mySock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET) {//creo socket
		printf("\nImpossibile creare il socket: %d", WSAGetLastError());
		return 1;
	}
	printf("Socket creato\n\n");
	myaddr_in.sin_family = AF_INET;
	myaddr_in.sin_addr.s_addr = INADDR_ANY;// inet_addr("127.0.0.1");
	myaddr_in.sin_port = htons(10000);//porta
	printf("ip server: %s\n", inet_ntoa(myaddr_in.sin_addr));
	printf("porta: %d\n\n\n", ntohs(myaddr_in.sin_port));
	if (bind(mySock, (struct sockaddr*)&myaddr_in, sizeof(struct sockaddr_in)) == -1) {//assegna un indirizzo al socket
		printf("Errore Bind\n");
		return 1;
	}


	int addrlen = sizeof(struct sockaddr_in);

	char msg[STANDARD_STRING_LENGTH];
	char saluto[STANDARD_STRING_LENGTH];
	//char client[STANDARD_STRING_LENGTH];
	//char tempString[STANDARD_STRING_LENGTH];
	int salutoScelto;

	

	do {
		// ricevo il comando dal client
		recvfrom(mySock, saluto, sizeof(char) * STANDARD_STRING_LENGTH, 0, (SOCKADDR*)&peeraddr_in, &addrlen);

		// converto il saluto scelto nel corrispettivo numerato (-1 se non riconosciuto)
		salutoScelto = getSaluto(saluto);

		// stampo le informazioni sul client inviante e il saluto inviato
		printf("Client: %s:%d\n", inet_ntoa(peeraddr_in.sin_addr), ntohs(peeraddr_in.sin_port));
		printf("Saluto: %s\n", saluto);

		// controllo se il saluto è giusto
		if (salutoScelto == salutoCorretto()) {
			printf("Esito: saluto corretto\n\n");

			// preparo il messaggio di esito positivo per il client
			strcpy(msg, "Saluto corretto!");
		}
		else {
			printf("Esito: saluto errato\n\n");

			// preparo il messaggio di esito negativo per il client
			strcpy(msg, "Saluto erratto!");
		}
		
		// invio l'esito al server
		sendto(mySock, msg, sizeof(char) * STANDARD_STRING_LENGTH, 0, (SOCKADDR*)&peeraddr_in, sizeof(peeraddr_in));
	} while (1);

	WSACleanup();
	system("PAUSE");
	return 0;
}


// la funzione salutoCorretto ritorna il valore numerico del saluto
int salutoCorretto() {
	// calcolo il tempo attuale
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	// in base all'ora attuale ritorno:
	//		0: se l'ora è compresa tra le 0 e le 12 (estremi inclusi)
	//		1: se l'ora è compresa tra le 13 e le 16 (estremi inclusi)
	//		2: se l'ora è compresa tra le 17 e le 23 (estremi inclusi)
	if (timeinfo->tm_hour < 13) {
		return 0;
	}
	else if (timeinfo->tm_hour >= 17) {
		return 2;
	}
	else {
		return 1;
	}
}

// la funzione getSaluto prende come parametro una stringa e la converte in un parametro numerico che indica univocamente un saluto
// se non riconosce il saluto passato ritorna -1
int getSaluto(char* saluto) {
	int i = 0;

	// confronto la stringa passata con le stringhe contenute nell'array saluti, che termina con la stringa "#"
	while (strcmp(saluti[i], "#") != 0) {
		// se la stringa i-esima corrisponde a quella passata, ritorno l'indice
		if (strcmp(saluto, saluti[i]) == 0) return i;
		i++;
	}

	// non è stato trovato nessun saluto corrispondente a quello passato, ritorno -1
	return -1;
}