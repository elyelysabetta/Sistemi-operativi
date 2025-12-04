// ./es info
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

void handlerCTRLC(){
    printf ("\nHo eseguito %d richieste!\n",nRichieste);
    exit (0);
}

int main (int argc,char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: numero argomenti errato (devi usare <file> <dir>)!\n");
        exit (1);
    }

    int fd=open(argv[1],O_DIRECTORY);
    if (fd<0){
        perror ("Errore directory!\n");
        exit (2);
    }
    close (fd);

    if (argv[1][0]=='/'){
        fprintf (stderr,"Errore: devi usare path relativo!\n");
        exit (3);
    }

    signal (SIGINT,handlerCTRLC);

    char tipo[DIM],data[DIM];
    pipee p1p2;
    int pid1,pid2,status;

    while(1){
        printf ("Inserisci tipologia di interesse: ");
        scanf ("%s",tipo);

        printf("Inserisci data di interesse: ");
        scanf ("%s",data); // formato YYYYMM

        if (strlen(data)!=6){
            fprintf (stderr,"Errore: formato data sbagliato (deve essere YYYYMM)!\n");
            exit (4);
        }
        for (int i=0;i<(int)strlen(data);i++)
            if (data[i]<'0' || data[i]>'9'){
                fprintf (stderr,"Errore: la data deve essere formata da cifre!\n");
                exit (5);
            }
        
        char pathFile[200];
        sprintf(pathFile,"%s/%s.txt",argv[1],data);
        int fd=open(pathFile,O_RDONLY);
        if (fd<0){
            perror("Errore file!\n");
            exit (6);
        }
        close (fd);

        if (pipe(p1p2)<0){
            perror("Errore nella pipe!\n");
            exit (7);
        }
        pid1=fork();
        if (pid1<0){
            perror("Errore nella fork di P1!\n");
            exit (8);
        }
        if (pid1==0){
            close (p1p2[0]); // P1 non legge da P2
            close(1); // chiudo stdout
            dup(p1p2[1]);
            close(p1p2[1]);

            execlp("grep","grep","-w",tipo,pathFile,(char*)0);
            perror("Errore nella exec grep!\n");
            exit(9);
        }
        close (p1p2[1]);
        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork di P2!\n");
            exit (10);
        }
        if (pid2==0){
            close (0);
            dup (p1p2[0]);
            close(p1p2[0]);

            execlp("sort","sort","-r","-n",(char*)0);
            perror("Errore nella exec sort!\n");
            exit (11);
        }
        close (p1p2[0]);
        nRichieste++;
        wait(&status);
        wait (&status);

    }
    return 0;


}