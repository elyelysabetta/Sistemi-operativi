/*Questo programma cerca caratteri (passati come parametri) in un file usando più processi figli, con un limite di tempo.
Crea un figlio per ogni carattere da cercare. Ogni figlio attende un segnale (SIGUSR1) per iniziare la ricerca.
Se un figlio finisce prima del tempo, avvisa gli altri con SIGUSR2 (terminazione anticipata).
Se scade il tempo (Nsec), il padre forza la terminazione con SIGUSR2.
Ogni figlio stampa quante volte ha trovato il suo carattere. Se un figlio termina prima, dice "ha vinto!".*/

// ./esercizio22 c1 c2 ... cn nomefile Nsec
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define DIM 1024

int CHAR_FOUND;          // numero di caratteri trovati dal figlio
int TERMINATION = 0;     // flag che segnala una terminazione anticipata
char CARATTERE;          // carattere da cercare (diverso per ogni figlio)

// gestore per SIGUSR1: inizio ricerca
void handlerStart(int segnale) {
    printf("Figlio %d riceve il segnale %d - carattere: %c\n", getpid(), segnale, CARATTERE);
}

// gestore per SIGUSR2: fine ricerca
void handlerEnd(int segnale) {
    printf("Figlio %d riceve il segnale %d - carattere %c trovato %d volte\n", getpid(), segnale, CARATTERE, CHAR_FOUND);
    exit(0);
}

// gestore per terminazione anticipata (usato dal padre)
void handlerEarly() {
    TERMINATION = 1;
}

int main(int argc, char **argv) {
    if (argc <= 4) {
        fprintf(stderr, "Errore: numero argomenti passati errato!\n");
        exit(1);
    }

    int numFigli = argc - 3;

    // Verifica che ogni argomento c1, c2, ..., cn sia un singolo carattere
    for (int i = 1; i <= numFigli; i++) {
        if (strlen(argv[i]) != 1) {
            fprintf(stderr, "Errore: l'argomento deve essere un singolo carattere!\n");
            exit(2);
        }
    }

    char filename[DIM]; // Nome del file
    strcpy(filename, argv[argc - 2]);

    // Numero di secondi per la ricerca
    int sec = atoi(argv[argc - 1]);
    if (sec <= 0) {
        fprintf(stderr, "Errore: i secondi devono essere un intero positivo!\n");
        exit(4);
    }

    // Imposta i gestori dei segnali per i figli
    signal(SIGUSR1, handlerStart);
    signal(SIGUSR2, handlerEnd);

    int pid, status;
    char charr;

    // Generazione figli
    for (int i = 0; i < numFigli; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Errore creazione figlio");
            exit(5);
        }

        if (pid == 0) {
            // Codice figlio
            CARATTERE = argv[i + 1][0];
            printf("Il figlio %d attende il segnale...\n", getpid());

            pause(); // attende SIGUSR1 per iniziare

            int fd = open(filename, O_RDONLY);
            if (fd < 0) {
                perror("Errore apertura file nel figlio");
                exit(6);
            }

            CHAR_FOUND = 0;
            while (read(fd, &charr, 1) > 0) 
                if (charr == CARATTERE)
                    CHAR_FOUND++;
            

            close(fd);

            // Se il figlio termina da solo, significa che ha vinto
            printf("Figlio %d ha vinto!\n", getpid());
            kill(0, SIGUSR2); // invia SIGUSR2 a tutti i processi
            exit(0);
        }
    }

    // Codice del padre (fuori dal ciclo)
    signal(SIGUSR1, SIG_IGN);        // il padre ignora SIGUSR1
    signal(SIGUSR2, handlerEarly);   // il padre gestisce SIGUSR2 (early termination)

    printf("Preparo...\n");
    sleep(sec); // tempo di attesa prima della ricerca

    // Inizio ricerca
    kill(0, SIGUSR1); // invia il segnale di inizio a tutti
    printf("Parto!\n");

    // Attende altri sec secondi
    sleep(sec);

    if (TERMINATION == 0) {
        printf("Tempo per la ricerca terminato!\n");
        kill(0, SIGUSR2); // invia il segnale di fine a tutti
    } else {
        printf("Terminazione anticipata!\n");
    }

    // Attende la terminazione dei figli
    for (int i = 0; i < numFigli; i++) {
        wait(&status);
    }

    return 0;
}
