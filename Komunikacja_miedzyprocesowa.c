#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
  
//Operacje na semaforze
#define ZAPIS 0
#define ODCZYT 1  
  
// Struktura wiadomosci (do kolejki komunikatow)
struct mesg_buffer 
{
    long mesg_type;
    char mesg_text[1024];
} message;

key_t key;
int msgid;
int shmemid; // Id pamieci wspoldzielonej
char *shmem; // Wskaznik na miejsce w pamieci
int tablicaSEM; // Id tablicy semaforow 

// Podnoszenie
void podnies(int numer)
{
	struct sembuf sem = {numer, 1, 0};
	semop(tablicaSEM, &sem, 1);
}

// Opuszczenie semafora
void opusc(int numer)
{
	struct sembuf sem = {numer, -1, 0};
	semop(tablicaSEM, &sem, 1);
}

int main(void)
{
  
    //Utworzenie unikalnego klucza IPC
    key = ftok("projekt.c", 65);
  
    // Utworzenie kolejki komunikatow
    msgid = msgget(key, 0666 | IPC_CREAT);
    message.mesg_type = 1;

	//Tworzenie tablicy seamforow
	//Argumenty - klucz tworzacy <wazne zeby byl unikalny>, liczba semaforow, flaga tworzaca
    tablicaSEM = semget(ftok(".", '0'), 2, 0777 | IPC_CREAT);

    // Poczatkowe wartosci semaforow 
	semctl(tablicaSEM, ZAPIS, SETVAL, 1);
	semctl(tablicaSEM, ODCZYT, SETVAL, 0);
	
	//Utworzenie pamieci wspoldzielonej 
	shmemid = shmget(ftok(".", '0'), 1024,  0777 | IPC_CREAT);
	shmem = (char*) shmat(shmemid, NULL, 0); 
	
	//PP1
	if(fork() == 0)
	{
		while(fgets(message.mesg_text,sizeof(message.mesg_text),stdin)!=NULL)
		{
			msgsnd(msgid, &message, sizeof(message.mesg_text), 0);
		}
		usleep(10000);	// Aby PP2 odczytal ostatni komunikat 
		// Usuniecie kolejki komunikatow
		msgctl(msgid, IPC_RMID, NULL);
		return 0;
	}
	//PP2
	if(fork() == 0)
	{
		usleep(10000);	// Aby PP1 zdazyl wyslac sygnal
		while(msgrcv(msgid, &message, sizeof(message.mesg_text), 0, 1)!=-1)
		{	
			int len = strlen(message.mesg_text);	// Ilosc odczytanych bajtow
			fprintf(stderr, " %d ", len);
			opusc(ZAPIS);
			memcpy(shmem, message.mesg_text, sizeof(message.mesg_text)); 
			podnies(ODCZYT);
		}
		// Wyslanie wiadomosci ktora zakonczy odczyt w PP3
		opusc(ZAPIS);
		memcpy(shmem, "KONIEC_ODCZYTU", 14); 
		podnies(ODCZYT);
		usleep(10000); // Aby PP3 zdazyl odczytac
		// Odlaczenie i usuniecie pamieci wspoldzielonej
		shmdt(shmem);
		shmctl(shmemid, IPC_RMID, NULL);
		return 0;
	}
	//PP3
	if(fork()==0)
	{
		usleep(20000);
		while(1)
		{
			opusc(ODCZYT);
			if(memcmp("KONIEC_ODCZYTU",shmem,7)==0)
			{
				break;
			};
			write(STDOUT_FILENO,shmem,strlen(shmem));
			podnies(ZAPIS);
		}
		 // Odlaczenie pamieci wspoldzielonej
		shmdt(shmem);
		return 0;
	}
	return 0;
}

