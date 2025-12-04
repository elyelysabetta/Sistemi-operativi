// ./es archivio_CdL
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#define DIM 1024

typedef int pipee[2];

int nRichieste=0;

void handlerExit(){
    printf ("\nHo eseguito %d richieste!\n",nRichieste);
    exit (0);
}

int main (int argc, char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: il numero di argomenti passati è errato!\n");
        exit(1);
    }

    if (argv[1][0]!='/'){
        fprintf (stderr,"Errore: devi usare il path assoluto!\n");
        exit (2);
    }

    int fd=open(argv[1],O_RDONLY);
    if (fd<0){
        perror ("Errore file!\n");
        exit (3);
    }

    signal(SIGINT, handlerExit);

    char insegnamento [DIM], risultato[DIM];
    pipee p1p2; // P1 scrive, P2 legge
    pipee p2p0; // P2 scrive, P0 legge
    int pid1,pid2,status;

    printf ("Inserisci nome coso che vuoi cercare: ");
    memset(insegnamento,0,sizeof(insegnamento));
    scanf ("%s",insegnamento);

    while(strcmp(insegnamento,"fine")!=0){
        if (pipe(p1p2)<0){
            perror("Errore nella pipe p1p2!\n");
            exit (4);
        }
        if (pipe(p2p0)<0){
            perror("Errore nella pipe p2p0!\n");
            exit (4);
        }

        pid1=fork();
        if (pid1<0){
            perror ("Errore nella fork (P1)!\n");
            exit (5);
        }
        if (pid1==0){ // P1
            close (p1p2[0]); // chiudo lettura P1
            close (1); 
            dup(p1p2[1]); 
            close(p1p2[1]);

            execlp("grep","grep","-w",insegnamento,argv[1],(char*)0);
            perror("Errore nella execlp grep!\n");
            exit(6);
        }
        close (p1p2[1]); // padre chiude scrittura p1p2

        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork (P2)!\n");
            exit (7);
        }
        if (pid2==0){ // P2
            close(0);
            dup(p1p2[0]);
            close (p1p2[0]);

            close (p2p0[0]); // chiudo lettura p2p0
            close (1);
            dup (p2p0[1]);
            close (p2p0[1]);

            execlp("sort","sort","-r",(char*)0);
            perror("Errore nella execlp sort!\n");
            exit (7);
        }

        close (p1p2[0]); 
        close (p2p0[1]); 

        int n;
        while((n = read(p2p0[0], risultato, sizeof(risultato))) > 0)
            printf("%s",risultato);

        close (p2p0[0]);
        wait (&status);
        wait(&status);

        nRichieste++;

        printf ("Inserisci nome coso che vuoi cercare: ");
        memset(insegnamento,0,sizeof(insegnamento));
        scanf ("%s",insegnamento);
    }

    printf ("Numero richieste: %d\n",nRichieste);
}
