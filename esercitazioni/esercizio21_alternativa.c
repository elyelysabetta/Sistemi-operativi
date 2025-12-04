#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int COUNTER=0;

void handler(int segnale){
    printf ("Il figlio %d ha avuto %d interazioni con il segnale %d\n",getpid(),COUNTER,segnale);
    exit (0);
}

int main (int argc, char **argv){
    if (argc!=3){
        fprintf (stderr,"Errore: numero argomenti passato non corretto!\n");
        exit (1);
    }

    int Nf=atoi(argv[1]);
    int Nsec=atoi(argv[2]);

    // controllo argomenti
    if (Nf<=0){
        fprintf (stderr,"Errore: primo argomento non valido (deve essere un intero positivo)!\n");
        exit (2);
    }
    if (Nsec<=0){
        fprintf (stderr,"Errore: secondo argomento non valido (deve essere un intero positivo)!\n");
        exit (3);
    }

    int *pid, status;
    pid=(int*)malloc(Nf *sizeof(int));
    for (int i=0;i<Nf;i++){
        pid[i]=fork();
        if (pid[i]<0){
            perror("Errore creazione figlio!\n");
            exit (4);
        }
        if (pid[i]==0){
            signal(SIGUSR1,handler);
            while(1){
                sleep(1);
                COUNTER++;
                // printf("sono figlio: %d, valore mio contatore: %d\n", getpid(), COUNTER);
            }
        }

        
    }

    sleep(Nsec); // il padre dorme

    for (int i=0;i<Nf;i++)
        kill(pid[i],SIGUSR1);
        
    free(pid);

    for (int i=0;i<Nf;i++)
        wait(&status);

}