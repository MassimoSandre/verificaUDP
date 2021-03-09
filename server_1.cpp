
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

const char* const commands[]{ "Hello","time","exit","#" };
int getCommand(char*);

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
	char command[STANDARD_STRING_LENGTH];
	//char client[STANDARD_STRING_LENGTH];
	char tempString[STANDARD_STRING_LENGTH];
	int comandoScelto;

	time_t rawtime;
	struct tm* timeinfo;

	do {
		// ricevo il comando dal client
		recvfrom(mySock, command, sizeof(char) * STANDARD_STRING_LENGTH, 0, (SOCKADDR*)&peeraddr_in, &addrlen);
		
		// stampo informazioni sul comando ricevuto e sul client che l'ha inviato
		

		comandoScelto = getCommand(command);

		switch (comandoScelto) {
		case -1:
			// nessun comando registrato corrisponde
			// la stringa sarà considerata un messaggio
			printf("Client: %s:%d\n", inet_ntoa(peeraddr_in.sin_addr), ntohs(peeraddr_in.sin_port));
			printf("Messaggio: %s\n\n", command);
			break;
		case 0:
			// il client vuole la data
			printf("Comando ricevuto: %s\nClient: %s:%d\n\n", command, inet_ntoa(peeraddr_in.sin_addr), ntohs(peeraddr_in.sin_port));
			
			strcpy(msg, "");

			// calcolo la data
			time(&rawtime);
			timeinfo = localtime(&rawtime);

			char tempString[STANDARD_STRING_LENGTH];

			// converto l'anno in stringa
			itoa(timeinfo->tm_year + 1900, tempString, 10);
			// aggiungo l'anno al messaggio
			strcat(msg, tempString);
			strcat(msg, "/");
			// converto il mese in stringa
			itoa(timeinfo->tm_mon + 1, tempString, 10);
			// aggiungo il mese al messaggio
			strcat(msg, tempString);
			strcat(msg, "/");
			// converto il giorno in stringa
			itoa(timeinfo->tm_mday, tempString, 10);
			// aggiungo il giorno al messaggio
			strcat(msg, tempString);


			// invio la data al client
			sendto(mySock, (char*)msg, sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&peeraddr_in, sizeof(peeraddr_in));
			
			break;
		case 1:
			// il client vuole l'ora
			printf("Comando ricevuto: %s\nClient: %s:%d\n\n", command, inet_ntoa(peeraddr_in.sin_addr), ntohs(peeraddr_in.sin_port));
			strcpy(msg, "");

			// calcolo l'ora
			time(&rawtime);
			timeinfo = localtime(&rawtime);

			// se il numero dell'ora è minore di 10 aggiungo uno zero prima
			if (timeinfo->tm_hour < 10) {
				strcat(msg, "0");
			}
			// converto l'ora in stringa
			itoa(timeinfo->tm_hour, tempString, 10);
			// aggiungo l'ora al messaggio
			strcat(msg, tempString);
			strcat(msg, ":");

			// se il numero dei minuti è minore di 10 aggiungo uno zero prima
			if (timeinfo->tm_min < 10) {
				strcat(msg, "0");
			}
			// converto i minuti in stringa
			itoa(timeinfo->tm_min, tempString, 10);
			// aggiungo i minuti al messaggio
			strcat(msg, tempString);
			strcat(msg, ":");

			// se il numero dei secondi è minore di 10 aggiungo uno zero prima
			if (timeinfo->tm_sec < 10) {
				strcat(msg, "0");
			}
			// converto i secondi in stringa
			itoa(timeinfo->tm_sec, tempString, 10);
			// aggiungo i secondi al messaggio
			strcat(msg, tempString);

			// invio l'ora al server
			sendto(mySock, msg, sizeof(char) * STANDARD_STRING_LENGTH, 0, (SOCKADDR*)&peeraddr_in, sizeof(peeraddr_in));
			break;
		case 2:
			// il client vuole terminare la comunicazione
			printf("Comando ricevuto: %s\nClient: %s:%d\n\n", command, inet_ntoa(peeraddr_in.sin_addr), ntohs(peeraddr_in.sin_port));

		default:
			// c'è stato un errore, la comunicazione verrà terminata in ogni caso
			comandoScelto = 2;
		}

	} while (comandoScelto!=2);

	WSACleanup();
	system("PAUSE");
	return 0;
}

// la funzione getCommand ritorna il valore numero associato ad ogni comand
// ritorna -1 se il comando passato non è riconosciuto come uno dei comandi registrati
int getCommand(char* command) {
	int i = 0;

	// leggo tutte le stringhe all'interno dell'array commands, che termina con la stringa "#"
	while (strcmp(commands[i], "#") != 0) {
		// se la stringa i-esita corrisponde al comando inserito, ritorno l'indice i
		if (strcmp(command, commands[i]) == 0) return i;
		i++;
	}
	// se il comando passato come parametro non è stato trovato nell'array dei comandi, ritorno -1
	return -1;
}