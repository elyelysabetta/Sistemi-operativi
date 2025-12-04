// ./es bollette.txt
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define DIM 1024
int numRichieste=0;
typedef int pipee[2];

void handler(){
    printf ("\nHai schiacciato ctrl-c. Totale richieste: %d\n",numRichieste);
    exit (0);
}

int main (int argc,char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: numero di argomenti errato!\n");
        exit (1);
    }

    // testo il file passato
    int fd=open(argv[1],O_RDONLY);
    if (fd<0){
        perror("Errore apertura file!\n");
        exit (2);
    }
    close(fd);
    if (argv[1][0]!='/'){
        fprintf (stderr,"Errore: devi passare il path assoluto!\n");
        exit (3);
    }

    char cerca[DIM];
    pipee p1p2,p2p3; 
    int pid1,pid2,pid3,status;

    signal(SIGINT,handler);

    do{
        printf ("Inserisci tipologia di bolletta (esci per terminare): ");
        scanf("%s",cerca);
        if (pipe(p1p2)<0){
            perror("Errore nella pipe p1p2!\n");
            exit(4);
        }

        pid1=fork();
        if (pid1<0){
            perror("Errore nella fork di P1!\n");
            exit (5);
        }
        if (pid1==0){ //sono nel figlio P1
            close (p1p2[0]);
            close (1); // chiudo lo stdout
            dup(p1p2[1]); // ridireziono uscita a p1p2
            close (p1p2[1]); 

            // P1 scrive su P2
            execlp("grep","grep","-w",cerca,argv[1],(char*)0);
            perror("Errore exec di P1!\n");
            exit (6);
        }
        close (p1p2[1]);

        if (pipe(p2p3)<0){
            perror ("Errore nella pipe p2p3!\n");
            exit (4);
        }

        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork di P2!\n");
            exit (5);
        }
        if(pid2==0){ // sono nel figlio P2
            // P2 legge da P1
            close (0); //chiudo stdin
            dup(p1p2[0]); // ridireziono imput 
            close (p1p2[0]);

            //P2 schrive su p3
            close (p2p3[0]); //P2 non legge da P3
            close (1); //chiudo stdout
            dup(p2p3[1]); //ridirigo output
            close(p1p2[1]);

            execlp("grep","grep","da pagare",(char*)0);
            perror("Errore nella exec 2!\n");
            exit (6);
        }

        close (p2p3[1]);
        pid3=fork();
        if (pid3<0){
            perror ("Errore nella fork di p3!\n");
            exit (5);
        }
        if (pid3==0){ // sono nel figlio P3
            close(0); //chiudo stdin
            dup(p2p3[0]); // ridirigo input
            close (p2p3[0]);

            //ordino dal maggiore al minore
            execlp("sort","sort","-r","-n",(char*)0);
            perror("Errore nella exec 3!\n");
            exit (6);
        }

        close (p2p3[0]);
        wait(&status); // P1
        wait (&status); // P2
        wait (&status); // P3

        numRichieste++;
    } while (strcmp(cerca, "esci")!=0);
    printf ("Hai inserito esci. Numero richieste: %d\n",numRichieste);
    return 0;
}