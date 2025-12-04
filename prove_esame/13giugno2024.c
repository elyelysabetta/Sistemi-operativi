// ./es Prestiti
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#define DIM 100

int nRichieste=0;
typedef int pipee[2];

void handlerC(int signo){
    printf ("\nHai eseguito %d richieste!\n",nRichieste);
    exit(4);
}

void handler1(){}

int main(int argc,char **argv){
    if (argc!=2){
        fprintf(stderr,"Errore: numero parametri errato!\n");
        exit (1);
    }

    int fd=open(argv[1],O_DIRECTORY);
    if (fd<0){
        perror("Errore directory!\n");
        exit (2);
    }
    close(fd);

    if (argv[1][0]=='/'){
        fprintf(stderr,"Errore: devi usare path relativo!\n");
        exit (3);
    }

    signal(SIGINT,handlerC);

    char codice[DIM],data[DIM];
    int N; //numero righe da visualizzare

    pipee p1p2,p2p3;
    int pid1,pid2,pid3,status;

    while(1){
        printf ("Inserisci codice libro: ");
        scanf("%s",codice);

        printf ("Inserisci data(formato YYYYMM): ");
        scanf ("%s",data);

        if(strlen(data)!=6){
            fprintf (stderr,"Errore: formato data sbagliato!\n");
            exit (4);
        }

        for (int i=0;i<(int)strlen(data);i++)
            if (data[i]<'0' || data[i]>'9'){
                fprintf (stderr,"Errore: la data può contenere solo cifre!\n");
                exit(5);
            }
        
        char pathFile[200];
        sprintf(pathFile,"%s/%s.txt",argv[1],data);
        int fd=open(pathFile,O_RDONLY);
        if(fd<0){
            perror("Errore file!\n");
            exit (6);
        }
        close (fd);

        printf("Inserisci numero righe da visualizzare: ");
        scanf("%d",&N);
        if (N<=0){
            fprintf (stderr,"Errore: il numero righe deve essere un intero positivo!\n");
            exit (7);
        }

        if (pipe(p1p2)<0){
            perror("Errore nella pipe p1p2!\n");
            exit (8);
        }

        pid1=fork();
        if (pid1<0){
            perror("Errore nella fork P1!\n");
            exit (9);
        }

        if(pid1==0){
            signal(SIGUSR1,handler1);
            pause();
            close(p1p2[0]); //P1 non legge da P2
            close(1);
            dup(p1p2[1]);
            close(p1p2[1]);

            execlp("grep","grep","-w",codice,pathFile,(char*)0);
            perror("Errore nella execgrep!\n");
            exit (10);
        }
        close(p1p2[1]);

        if(pipe(p2p3)<0){
            perror("Errore nella pipe P2P3!\n");
            exit (11);
        }
        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork P2!\n");
            exit(12);
        }
        if (pid2==0){
            close(0);
            dup(p1p2[0]);
            close(p1p2[0]);

            close(p2p3[0]); //P2 non legge da P3
            close(1);
            dup(p2p3[1]);
            close (p2p3[1]);

            execlp("sort","sort","-r",(char*)0);
            perror("Errore nella exec sort!\n");
            exit (13);
        }
        
        close (p1p2[0]);
        close(p2p3[1]);

        pid3=fork();
        if (pid3<0){
            perror("Errore nella fork P3!\n");
            exit (15);
        }
        if (pid3==0){
            close (0);
            dup (p2p3[0]);
            close (p2p3[0]);

            char nRighe[4];
            sprintf(nRighe,"%d",N);
            execlp("head","head","-n",nRighe,(char*)0);
            perror("Errore nella exec head!\n");
            exit (16);
        }
        close(p2p3[0]);

        kill(pid1,SIGUSR1);

        nRichieste++;
        wait(&status);
        wait(&status);
        wait(&status);
    }
    




}