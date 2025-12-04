// ./es 2006
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#define DIM 100

typedef int pipee[2];
int nRichieste=0;

void handlerC(){
    printf ("\nNumero richieste: %d\n",nRichieste);
    exit (0);
}

int main (int argc, char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: numero argomenti passati errato!\n");
        exit (1);
    }

    int fd=open(argv[1],O_DIRECTORY);
    if(fd<0){
        perror("Errore directory!\n");
        exit (2);
    }
    close (fd);

    if (argv[1][0]!='/'){
        fprintf (stderr,"Errore: devi usare il path relativo!\n");
        exit (3);
    }

    char nomefile[DIM],strada[DIM];
    pipee p1p2,p2p3;
    int pid1,pid2,pid3,status;

    signal(SIGINT,handlerC);

    while(1){
        printf ("Inserisci data in formato MMDD: ");
        scanf ("%s",nomefile);
        if((int)strlen(nomefile)!=4){
            fprintf (stderr,"Errore: dovevi usare il formato MMDD!\n");
            exit (4);
        }

        for (int i=0;i<(int)strlen(nomefile);i++)
            if (nomefile[i]<'0' || nomefile[i]>'9'){
                fprintf (stderr,"Errore: la data puo contenere sono cifre!\n");
                exit (5);
            }
        
        char pathFile[1000];
        sprintf(pathFile,"%s/%s.txt",argv[1],nomefile);
        int fd=open(pathFile,O_RDONLY);
        if (fd<0){
            perror("Errore file!\n");
            exit (6);
        }
        close (fd);

        printf ("Inserisci nome strada: ");
        scanf ("%s",strada);

        if (pipe(p1p2)<0){
            perror ("Errore nella pipe p1p2!\n");
            exit (7);
        }

        pid1=fork();
        if (pid1<0){
            perror("Errore nella fork P1!\n");
            exit (8);
        }
        if (pid1==0){
            close (p1p2[0]); //P1 non legge da P2
            close(1);
            dup(p1p2[1]);
            close (p1p2[1]);
            execlp("grep","grep","-w",strada,pathFile,(char*)0);
            perror("Errore nella grep!\n");
            exit (9);
        }
        close (p1p2[1]);

        if (pipe(p2p3)<0){
            perror("Errore nella pipe p1p2!\n");
            exit (10);
        }

        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork!\n");
            exit(11);
        }
        if (pid2==0){
            close (0);
            dup (p1p2[0]);
            close (p1p2[0]);

            close (p2p3[0]); //P2 non legge da P3
            close(1);
            dup(p2p3[1]);
            close (p2p3[1]);

            execlp("sort","sort",(char*)0);
            perror("Errore nella exec sort!\n");
            exit (12);
        }

        close(p1p2[0]);
        close (p2p3[1]);
        pid3=fork();
        if (pid3<0){
            perror ("Errore nella fork!\n");
            exit (13);
        }
        if (pid3==0){
            close (0);
            dup (p2p3[0]);
            close(p2p3[0]);
            execlp("cut","cut","-d,","-f1,2",(char*)0);
            perror("Errore nella exec cut!\n");
            exit (14);
        }
        close (p2p3[0]);
        nRichieste++;
        wait(&status);
        wait(&status);
        wait(&status);
    }
    
}
