/* Il programma chiede all'utente di inserire una matricola ("fine" per terminare).
Cerca la matricola in un file (specificato come argomento con path assoluto) e conta le occorrenze trovate.
Usa due processi figli e una pipe per comunicare tra loro: primo figlio (pid1) esegue grep e scrive i risultati nella pipe.
Secondo figlio (pid2) legge dalla pipe e conta le righe con wc -l.
Il padre attende la terminazione di entrambi i figli prima di ripetere il ciclo.*/

// ./esercizio31 /home/utente/esami.txt  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define DIM 1024

int main (int argc,char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: numero parametri passati errato!\n");
        exit (1);
    }

    char filename[DIM];
    strcpy (filename,argv[1]);

    if (filename[0]!='/'){ // verifico path assoluto
        fprintf(stderr,"Errore: devi usare il path assoluto!\n");
        exit (2);
    }

    // verifico che il file esista
    int fd=open(filename,O_RDONLY);
    if (fd<0){
        perror("Errore apertura file!\n");
        exit (3);
    }
    close (fd);

    char matricola[DIM];
    int p0[2]; // pipe con 2 estremi (lettura e scrittura)
    int pid1,pid2, status;
    do{
        printf ("Inserisci numero matricola (fine per terminare): ");
        scanf("%s",matricola);
        if (pipe(p0)<0){
            perror ("Errore nella pipe!\n");
            exit (4);
        }
        pid1=fork();
        if (pid1<0){
            perror("Errore nella pipe!\n");
            exit (5);
        }

        if (pid1==0){
            close (p0[0]); // chiudo estremo lettura (perche devo solo scrivere sulla pipe)
            close (1); // chiudo stdout
            dup (p0[1]); // duplico estremo scrittura della pipe sul primo fd libero (che è 1)
            close (p0[1]); // ora che l'ho duplicato chiudo l'originale scrittura

            execlp("grep","grep","-w",matricola,filename,(char*)0);
            perror("Errore nella exec!\n");
            exit(6);
        }

        pid2=fork();
        if (pid2<0){
            perror("Errore nella fork!\n");
            exit (5);
        }
        if (pid2==0){
            close(p0[1]); // chiudo estremo scrittura (perchè devo solo leggere dalla pipe)
            close (0); // chiudo lo stdin
            dup(p0[0]); //duplico estremo lettura della pipe sul primo fd libero (stdin)
            close (p0[0]); // ora che l'ho duplicato chiudo l'originale lettura

            if (strcmp(matricola,"fine")!=0){
                execlp("wc","wc","-l",(char*)0);
                perror("Errore nella exec!\n");
                exit (6);
            }
        }

        close (p0[0]);
        close (p0[1]);

        wait(&status);
        wait(&status);
    } while (strcmp(matricola,"fine")!=0);
    return 0;
}
