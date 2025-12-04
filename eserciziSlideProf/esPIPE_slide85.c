#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MSGSIZE 50  // Dimensione del messaggio

// Array di messaggi da inviare dal padre al figlio
char *msg[10] = {
    "Salve. Messaggio #0", "Salve. Messaggio #1", "Salve. Messaggio #2",
    "Salve. Messaggio #3", "Salve. Messaggio #4", "Salve. Messaggio #5",
    "Salve. Messaggio #6", "Salve. Messaggio #7", "Salve. Messaggio #8", "Salve. Messaggio #9"
};

int main() {
    int pid, j, piped[2], stato;
    char inpbuf[MSGSIZE];

    // Creazione della pipe
    if (pipe(piped) < 0) {
        perror("pipe");
        exit(1);
    }

    // Creazione del processo figlio
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(2);
    }

    if (pid > 0) {  // Processo padre
        close(piped[0]);  // Il padre chiude il lato lettura della pipe

        // Scrittura dei messaggi nella pipe
        for (j = 0; j < 10; j++) {
            write(piped[1], msg[j], MSGSIZE);  // Scrittura del messaggio nella pipe
        }

        // Aspetta la terminazione del figlio
        wait(&stato);
        exit(0);

    } else {  // Processo figlio
        close(piped[1]);  // Il figlio chiude il lato scrittura della pipe

        // Lettura dei messaggi dalla pipe
        for (j = 0; j < 10; j++) {
            read(piped[0], inpbuf, MSGSIZE);  // Lettura del messaggio dalla pipe
            printf("Figlio legge: %s\n", inpbuf);  // Stampa del messaggio letto
        }
    }

    return 0;
}
