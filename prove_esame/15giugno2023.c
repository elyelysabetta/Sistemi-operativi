// ./es dir
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h> // per le directory

#define DIM 10
typedef int pipee[2];

void handler(){}

int main (int argc, char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: numero di argomenti passati errato!\n");
        exit(1);
    }

    if(argv[1][0]=='/'){
        fprintf(stderr,"Errore: devi usare il path relativo!\n");
        exit(2);
    }

    int fd=opendir(argv[1]);
    if (fd==NULL){
        perror("Errore: la directory non esiste!\n");
        exit (3);
    }
    close (fd);

    signal(SIGUSR2,handler);

    char IDaereo[DIM], data[DIM];
    char pathFile[200];
    int Nvoli;
    int pid1,pid2,pid3, status;
    pipee p1p2,p2p3;

    do{
        printf("Inserisci id aereo che vuoi cercare (fine per terminare): ");
        scanf ("%s",IDaereo);

        if (strcmp(IDaereo,"fine")==0)
            break;

        sprintf(pathFile, "%s/%s.txt", argv[1], IDaereo); //path diventa <dir>/<IDaereo>.txt
        // verifico file
        int fd=open(pathFile,O_RDONLY);
        if (fd<0){
            perror ("Errore apertura file!\n");
            exit (4);
        }
        close(fd);

        printf ("Inserisci data (formato GGMMAAAA): ");
        scanf("%s",data);

        // controlli sulla data
        if (strcmp(data,"fine")==0)
            break;
        if (strlen(data)!=8){
            fprintf (stderr,"Errore: formato data non valido!\n");
            exit (5);
        }

        printf ("Inserisci numero voli che vuoi visualizzare: ");
        scanf ("%d",&Nvoli);

        if (Nvoli<=0){
            fprintf (stderr,"Errore: numero righe non valido (deve essere un intero positivo)!\n");
            exit(6);
        }

        if (pipe(p1p2)<0){
            perror("Errore nella pipe p1p2!\n");
            exit (7);
        }
        
        pid1=fork();
        if(pid1<0){
            perror("Errore nella fork di P1!\n");
            exit (8);
        }
        if (pid1==0){ // sono nel figlio P1
            signal(SIGUSR2,SIG_DFL); // comportamento di default
            close (p1p2[0]); //P1 solo scrive su P2
            close (1); //chiudo stdout
            dup (p1p2[1]); //ridirigo output
            close (p1p2[1]);

            execlp("grep","grep","-w",data,pathFile,(char*)0);
            perror("Errore nella exec di P1!\n");
            exit (9);
        }
        close (p1p2[1]);

        if (pipe(p2p3)<0){
            perror ("Errore nella pipe p2p3");
            exit (7);
        }

        pid2=fork();
        if(pid2<0){
            perror ("Errore nella fork di P2!\n");
            exit (8);
        }

        if (pid2==0){ // sono nel figlio P"
            signal(SIGUSR2,SIG_DFL);
            close(0); //chiudo stdin
            dup (p1p2[1]); // ridireziono input
            close (p1p2[1]);

            close (p2p3[0]); // P2 non legge da P3, ma solo scrive
            close (1); //chiudo stdout
            dup(p2p3[1]); // ridireziono output
            close (p2p3[1]);

            execlp("sort","sort","-n",(char*)0);
            perror("Errore nella exec di P2!\n");
            exit (9);
        }
        close (p2p3[1]);

        pid3=fork();
        if (pid3<0){
            perror("Errore nella fork di P3!\n");
            exit(8);
        }
        if (pid3==0){ // sono nel figlio P3
            signal(SIGUSR2,SIG_DFL);
            kill(getpid(),SIGUSR2);
            close (0); //chiudo stdin
            dup (p2p3[0]);
            close (p2p3[0]);

            execlp("tail","tail","-n",(char*)0);
            perror("Errore nella exec di P3");
            exit (9);
        }
        close (p2p3[0]);

        wait (&status); // per P1
        wait (&status); // per P2
        wait (&status); // per p3


    } while (strcmp(IDaereo,"fine")!=0); // posso anche mettere while(1) perche faccio gia dei controlli interni




}