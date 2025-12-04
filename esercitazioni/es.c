#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#define DIM 1024

char CHARACHTER;
int TERMINATION=0;
int COUNTER;


void handlerStart(int sig){
    printf ("Il figlio %d riceve il segnale %d - carattere %c\n",getpid(),sig,CHARACHTER);
}

void handlerEnd(int sig){
    printf ("Il figlio %d riceve il segnale %d - carattere %c trovato %d volte\n", getpid(),sig,CHARACHTER,COUNTER);
}

void handlerEarly(int sig){
    TERMINATION=1;
}

int main (int argc, char **argv){
    if (argc<=4){
        fprintf (stderr,"Errore: numero di argomenti passati non corretto!\n");
        exit (1);
    }

    int numFigli=argc-3;
    if (numFigli<2){
        fprintf (stderr,"Errore parametro passato: i caratteri da cercare devono essere almeno 2!\n");
        exit (2);
    }

    for (int i=0;i<numFigli;i++)
        if (strlen(argv[i+1])!=1){
            fprintf (stderr,"Errore: parametro passato errato (deve essere un carattere)\n");
            exit (3);
        }

    char nomefile[1024];
    strcpy(nomefile, argv[argc-2]);

    int sec=atoi(argv[argc-1]);
    if (sec<=0){
        fprintf (stderr, "Errore parametro passato: i secondi devono essere un numero intero positivo!\n");
        exit (4);
    }

    signal (SIGUSR1,handlerStart);
    signal (SIGUSR2,handlerEnd);

    int pid, status;
    char charr;
    for (int i=0;i<numFigli;i++){
        pid=fork();
        if (pid<0){
            perror("Errore nella fork!\n");
            exit (6);
        }

        if (pid==0){
            int fd=open(nomefile,O_RDONLY);
            if (fd<0){
                perror("Errore apertura file!\n");
                exit (5);
            }
            
            signal(SIGUSR1, handlerStart);
            signal(SIGUSR2, handlerEnd);

            CHARACHTER=argv[i+1][0];
            COUNTER=0;
            while(read(fd,&charr,1)>0)
                if (charr==CHARACHTER)
                    COUNTER++;

            printf ("Il figlio %d ha vinto!\n",getpid());
            kill(0,SIGUSR2);
            exit(0);
        }
    }
    close (fd);

    signal(SIGUSR1,SIG_IGN);
    signal(SIGUSR2,handlerEarly);

    printf ("Preparo..\n");
    sleep(sec);

    printf ("Parto..\n");
    kill(0,SIGUSR1);

    sleep(sec);

    if (TERMINATION==1)
        printf("Fine anticipata!\n");
    else {
        printf ("Il tempo disponibile è terminato!\n");
        kill(0,SIGUSR2);
    }

    for (int i=0;i<numFigli;i++)
        wait (&status);




}