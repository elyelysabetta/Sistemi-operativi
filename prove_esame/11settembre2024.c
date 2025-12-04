// ./es Ordini
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
    printf ("\nNumero richieste: %d\n",nRichieste);
    exit (0);
}

int main (int argc,char **argv){

    if (argc!=2){
        fprintf (stderr,"Errore: numero argomenti passati errato!\n");
        exit(1);
    }

    int fd=open(argv[1],O_DIRECTORY);
    if (fd<0){
        perror("Errore directory!\n");
        exit (2);
    }
    close (fd);

    if (argv[1][0]!='/'){
        fprintf (stderr,"Errore: devi usare il path assoluto!\n");
        exit (3);
    }

    char corriere[DIM],data[DIM];
    int N;
    pipee p1p2,p2p3,p3p0;
    int pid1,pid2,pid3, status;

    signal(SIGINT,handlerC);

    while(1){
        printf ("Inserisci ID corriere: ");
        scanf ("%s",corriere);

        printf("Inserisci data (YYYMMDD): ");
        scanf ("%s",data);
        if (strlen(data)!=8){
            fprintf (stderr,"Errore: formato data sbagliato!\n");
            exit (4);
        }
        for (int i=0;i<(int)strlen(data);i++)
            if (data[i]<'0' || data[i]>'9'){
                fprintf(stderr,"Errore: la data può contenere solo cifre!\n");
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

        printf ("Inserisci numero righe che vuoi visualizzare: ");
        scanf("%d",&N);
        if(N<=0){
            fprintf(stderr,"Errore: il numero di righe deve essere un intero positivo!\n");
            exit (7);
        }

        if (pipe(p1p2)<0){
            perror ("Errore nella pipe p1p2!\n");
            exit (8);
        }

        pid1=fork();
        if(pid1<0){
            perror("Errore nella fork P1!\n");
            exit (9);
        }
        if (pid1==0){
            close (p1p2[0]); // P1 non legge da P2
            close (1);
            dup(p1p2[1]);
            close(p1p2[1]);

            execlp("grep","grep","-w",corriere,pathFile,(char*)0);
            perror("Errore exec grep!\n");
            exit (10);
        }
        close (p1p2[1]);


        if(pipe(p2p3)<0){
            perror("Errore nella pipe p2p3!\n");
            exit (11);
        }

        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork P2!\n");
            exit (12);
        }
        if (pid2==0){
            close(0);
            dup (p1p2[0]);
            close(p1p2[0]);

            close(p2p3[0]); //P2 non legge da P3
            close(1);
            dup(p2p3[1]);
            close(p2p3[1]);

            execlp("sort","sort",(char*)0);
            perror("Errore nella exec sort!\n");
            exit(13);
        }

        close(p1p2[0]);
        close(p2p3[1]);

        if(pipe(p3p0)<0){
            perror("Errore nella pipe P3P0!\n");
            exit(14);
        }

        pid3=fork();
        if (pid3<0){
            perror("Errore nella fork P3!\n");
            exit (15);
        }
        if (pid3==0){
            close (0);
            dup(p2p3[0]);
            close (p2p3[0]);

            close (p3p0[0]); //P3 non legge dal padre
            close(1);
            dup (p3p0[1]);
            close(p3p0[1]);

            char nRighe[4];
            sprintf(nRighe,"%d",N);
            execlp("tail","tail","-n",nRighe,(char*)0);
            perror("Errore nella exec tail!\n");
            exit (16);
        }
        close(p2p3[0]);
        close (p3p0[1]);

        char riga[500];
        while(read(p3p0[0],riga,sizeof(riga))>0)
            write(1,riga,strlen(riga));
        
        close (p3p0[0]);

        nRichieste++;
        wait(&status);
        wait(&status);
        wait(&status);
       
    }


}
