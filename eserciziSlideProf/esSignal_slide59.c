#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void gestore(int segnale) {
    printf("Ricevuto segnale: %d\n", segnale);
}

int main() {
    // Associa SIGINT (Ctrl+C) al gestore personalizzato
    signal(SIGINT, gestore);

    // Loop infinito
    for (int i=0;i<15;i++) {
        printf("In attesa di un segnale...\n");
        sleep(3); // stampa il messaggio ogni 5 secondi
    }

    return 0;
}



