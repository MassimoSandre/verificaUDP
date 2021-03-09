
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

struct sockaddr_in peeraddr_in;
struct sockaddr_in myaddr_in;
struct hostent* hp;

const char* const commands[]{ "Hello","time","exit","#" };
int getCommand(char*);


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

		char command[STANDARD_STRING_LENGTH];
		char msg[STANDARD_STRING_LENGTH];
		int comandoScelto;
		
		do {
			printf("VERIFICA SOCKET UDP - MASSIMO ALBINO SANDRETTI\nLista comandi:\n\tHello: ricevi la data\n\ttime: ricevi l'ora\n\texit: termina la connessione");
			do {
				printf("\n\nInserire un comando: ");
				// leggo un comando
				scanf("%s", command);
				// converto il comando testuale nel corrispettivo numerico
				comandoScelto = getCommand(command);
				// l'utente non ha il permesso di forzare l'invio di un messaggio
				if (comandoScelto == -1) {
					system("CLS");
					printf("VERIFICA SOCKET UDP - MASSIMO ALBINO SANDRETTI\nLista comandi:\n\tHello: ricevi la data\n\ttime: ricevi l'ora\n\texit: termina la connessione\n");
					printf("\nInserire un comando valido!");
				}
			} while (comandoScelto == -1);

			printf("\n\n");

			switch (comandoScelto) {
			case 0:
				// comando -> Hello
				// il client vuole la data

				// invio al server il comando scelto
				sendto(mySock, commands[comandoScelto], sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&myaddr_in, sizeof(myaddr_in));

				// ricevo una stringa dal server: la data di oggi
				recvfrom(mySock, msg, sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&peeraddr_in, &structSize);

				// stampo la data, così che l'utente possa visualizzarla
				printf("Data: %s\n", msg);
				break;
			case 1:
				// comando -> time
				// il client vuole la data

				// invio al server il comando scelto
				sendto(mySock, commands[comandoScelto], sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&myaddr_in, sizeof(myaddr_in));

				// ricevo una stringa dal server: l'ora attuale
				recvfrom(mySock, msg, sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&peeraddr_in, &structSize);

				// stampo la ora, così che l'utente possa visualizzarla
				printf("Ora: %s\n", msg);

				// l'ora è nel formato HH:MM:SS, quindi, attingendo ai primi 2 caratteri, è possibile estrarre il valore dell'ora
				int ora;
				ora = (msg[0] - 48) * 10 + (msg[1] - 48);

				if (ora > 18) {
					// bisogna mandare un messaggio al server, qualsiasi stringa non riconosciuta come comando dal server sarà interpretata come messaggio da parte del client e sarà stampato a video
					strcpy(msg, "Buonasera");
					sendto(mySock, msg, sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&myaddr_in, sizeof(myaddr_in));
				}
				break;
			case 2:
				// comando -> exit
				// il client vuole terminare la comunicazione
			default:
				// c'è stato un errore nell'inserzione del comando
				// la comunicazione viene terminata

				comandoScelto = 2;

				// invio al server il comando scelto
				sendto(mySock, commands[comandoScelto], sizeof(char) * STANDARD_STRING_LENGTH, 0, (struct sockaddr*)&myaddr_in, sizeof(myaddr_in));
			}

			if (comandoScelto != 2) {
				printf("\n\n");
				system("PAUSE");
				system("CLS");
			}
		} while (comandoScelto != 2);
		
	}

	printf("Comunicazione interrotta\n");

	system("PAUSE");
}

// la funzione getCommand ritorna il valore numero associato ad ogni comand
// ritorna -1 se il comando passato non è riconosciuto come uno dei comandi registrati
int getCommand(char* command) {
	int i = 0;
	
	// leggo tutte le stringhe all'interno dell'array commands, che termina con la stringa "#"
	while (strcmp(commands[i], "#")!=0) {
		// se la stringa i-esita corrisponde al comando inserito, ritorno l'indice i
		if (strcmp(command, commands[i])==0) return i;
		i++;
	}
	// se il comando passato come parametro non è stato trovato nell'array dei comandi, ritorno -1
	return -1;
}