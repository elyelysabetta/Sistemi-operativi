/*Crea Nf processi figli (specificato come primo argomento). Ogni figlio icrementa un contatore 
(CONTATORE) ogni secondo e si ferma solo quando riceve un segnale SIGUSR1 dal padre.
Il padre attende Nsec secondi (secondo argomento). Dopo il tempo stabilito, invia SIGUSR1 a tutti i figli per terminarli.
Ogni figlio, prima di terminare, stampa il numero di incrementi effettuati.*/

// ./esercizio21 Nf Nsec
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // per fork, sleep, getpid..
#include <sys/wait.h>
#include <signal.h> // per signal e kill

int CONTATORE=0; // numero di interazioni

void handler(int segnale){
    printf ("Il figlio %d ha eseguito %d interazioni per il segnale %d\n",getpid(),CONTATORE,segnale);
    exit (0);
}

int main (int argc, char **argv){
    if (argc!=3){
        fprintf (stderr,"Errore: numero di argomenti passati errato!\n");
        exit (1);
    }

    int Nf=atoi(argv[1]);
    int Nsec=atoi (argv[2]);

    //controllo parametri 
    if (Nf<=0){
        fprintf(stderr,"Errore: numero processi non valido (devi inserire un intero positivo)!\n");
        exit(2);
    }
    if(Nsec<=0){
        fprintf(stderr,"Errore: numero secondi non valido (devi inserire un intero positivo)!\n");
        exit(3);
    }
    int pid[Nf], status;

    for (int i=0;i<Nf;i++){
        pid[i]=fork();
        if (pid[i]<0){
            perror("Errore creazione figlio!\n");
            exit (4);
        }
        if (pid[i]==0){
            // gestisco il segnale SIGUSR1 con il comportamento indicato in handler 
            signal(SIGUSR1,handler);
            while(1){
                sleep(1);
                CONTATORE++;
                // printf ("Figli %d: valore contatore %d\n",getpid(),CONTATORE);
            }
        }
    }

    //il padre va fuori dal for, perchè se fosse dentro generava un figlio e lo uccide, poi genera e uccide
    //così facendo invece, lui genera tutti i figli e poi li uccide tutti insieme
    //(ai fini dell output non sarebbe cambiato nulla)
    sleep(Nsec); //padre

    for (int i=0;i<Nf;i++)
        kill(pid[i],SIGUSR1); // invia SIGUSR1 a tutti i figli

    for (int i=0;i<Nf;i++)
        wait(&status);

}