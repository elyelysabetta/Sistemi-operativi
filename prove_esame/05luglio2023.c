// ./es Biblioteca
// il codice non stampa!!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#define DIM 100

int nRichieste=0;
typedef int pipee[2];

void handlerC(){
    printf ("\nTotale richieste: %d\n",nRichieste);
    exit (0);
}

void handler1(){}

int main (int argc, char **argv){
    if (argc!=2){
        fprintf(stderr,"Errore: numero di argomenti passati errato!\n");
        exit (1);
    }

    int fd=open(argv[1],O_DIRECTORY);
    if(fd<0){
        perror("Errore directory!\n");
        exit(2);
    }
    close (fd);

    if (argv[1][0]!='/'){
        fprintf (stderr,"Errore: devi usare path assoluto!\n");
        exit (3);
    }

    char IDutente[DIM];
    int N;
    pipee p1p2,p2p3,p3p0;
    int pid1,pid2,pid3,status;

    signal(SIGINT,handlerC);

    while(1){
        printf ("Inserisci ID utente: ");
        scanf("%s",IDutente);

        char pathFile[200];
        sprintf(pathFile,"%s/%s.txt",argv[1],IDutente);
        int fd=open(pathFile,O_RDONLY);
        if(fd<0){
            perror("Errore file!\n");
            exit (4);
        }
        close (fd);

        printf ("Inserisci numero righe da visualizzare: ");
        scanf ("%d",&N);
        if (N<=0){
            fprintf (stderr,"Errore: devi inserire un intero positivo!\n");
            exit(-1);
        }

        if (pipe(p1p2)<0){
            perror("Errore nella pipe!\n");
            exit(5);
        }
        pid1=fork();
        if (pid1<0){
            perror("Errore nella fork!\n");
            exit (6);
        }
        if (pid1==0){
            signal(SIGUSR1,handler1);
            pause();
            
            close(p1p2[0]); // P1 non legge da P2
            close(1);
            dup (p1p2[1]);
            close (p1p2[1]);

            execlp("sort","sort","-n",pathFile,(char*)0);
            perror("Errore nella exec sort!\n");
            exit (7);
        }
        close (p1p2[1]);

        if (pipe(p2p3)<0){
            perror("Errore nella pipe p2p3!\n");
            exit (8);
        }
        pid2=fork();
        if (pid2<0){
            perror ("Errore nella fork P2!\n");
            exit (9);
        }
        if (pid2==0){
            close(0);
            dup (p1p2[0]);
            close(p1p2[0]);

            close (p2p3[0]); // P2 non legge da P3
            close(1);
            dup (p2p3[1]);
            close (p2p3[1]);
            
            execlp("grep","grep","-w","NONRESTITUITO",(char*)0);
            perror("Errore nella exec grep!\n");
            exit (10);
        }
        close (p1p2[0]);
        close (p2p3[1]);

        if (pipe(p3p0)<0){
            perror("Errore nella pipe p3p0!\n");
            exit(11);
        }

        pid3=fork();
        if (pid3<0){
            perror("Errore nella fork P3!\n");
            exit (12);
        }
        if (pid3==0){
            close(0);
            dup(p2p3[0]);
            close(p2p3[0]);

            close(p3p0[0]); // P3 non legge dal padre
            close(1);
            dup(p3p0[1]);
            close(p3p0[1]);

            char nRighe[4];
            sprintf(nRighe,"%d",N);
            execlp("head","head","-n",nRighe,(char*)0);
            perror("Errore nella exec head!\n");
            exit (13);
        }
        close (p2p3[0]);
        close (p3p0[1]);

        kill(pid1,SIGUSR1);

        char riga[500];
        while(read(p3p0[0],riga,sizeof(riga))>0)
            printf("%s",riga);

        close(p3p0[0]);
    
        nRichieste++;
        wait(&status);
        wait(&status);
        wait(&status);
    }
    

}
