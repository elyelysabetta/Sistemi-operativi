// ./es fileAnagrafica.txt
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#define DIM 100

int nRichieste=0;

void handlerCTRLC(){
    printf ("Hai usato CTRL-C! Tot richieste: %d\n",nRichieste);
    exit (0);
}

int main (int argc, char **argv){
    if(argc!=2){
        fprintf (stderr,"Errore: numero argomenti passati errato!\n");
        exit(1);
    }

    int fd=open(argv[1],O_RDONLY);
    if (fd<0){
        perror("Errore file!\n");
        exit (2);
    }
    close (fd);

    if (argv[1][0]=='/'){
        fprintf (stderr,"Errore: devi usare path assoluto!\n");
        exit (3);
    }

    char nome[DIM],prefisso[4];
    int p1p2[2];
    int pid1,pid2,status;

    signal(SIGINT,handlerCTRLC);

    while(1){
        printf ("Inserisci nome persona (fine per terminare): ");
        scanf ("%s",nome);
        if (strcmp(nome,"fine")==0)
            break;
        
        printf("Inserisci prefisso (fine per terminare): ");
        scanf ("%s",prefisso);
        if (strcmp(prefisso,"fine")==0)
            break;
        for (int i=0;i<(int)strlen(prefisso);i++)
            if (prefisso[i]<'0' || prefisso[i]>'9'){
                fprintf (stderr,"Errore: il prefisso puo contenere solo cifre!\n");
                exit (4);
            }

        if (pipe(p1p2)<0){
            perror("Errore nella pipe!\n");
            exit (5);
        }

        pid1=fork();
        if(pid1<0){
            perror("Errore fork P1!\n");
            exit (6);
        }
        if (pid1==0){
            close (p1p2[0]); //P1 non legge da P2
            close(1);
            dup (p1p2[1]);
            close (p1p2[1]);

            execlp("grep","grep","-w",nome,argv[1],(char*)0);
            perror("Errore nella grep nome!\n");
            exit (7);
        }
        close(p1p2[1]);
        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork P2!\n");
            exit (8);
        }
        if (pid2==0){
            close (0);
            dup(p1p2[0]);
            close(p1p2[0]);

            execlp("grep","grep","-w","-n",prefisso,(char*)0);
            perror("Errore exec grep prefisso!\n");
            exit (9);
        }
        close(p1p2[0]);

        nRichieste++;
        wait(&status);
        wait (&status);
    }

    printf ("Hai inserito fine! Tot richieste: %d\n",nRichieste);
}