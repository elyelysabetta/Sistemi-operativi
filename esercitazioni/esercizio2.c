/*Questo programma chiede all'utente di inserire il nome di un file e ne mostra le prime N righe 
(dove N è passato come argomento). Ripete l'operazione finché non viene digitato "fine"*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#define dim 1024

int main (int argc, char **argv){
    if (argc!=2){
        fprintf (stderr, "Errore: numero argomenti passati errato!\n");
        exit (1);
    }

    int numerorighe=atoi(argv[1]);
    if (numerorighe<=0){
        fprintf (stderr,"Errore: argomento non valido (deve essere un intero >0)!\n");
        exit(2);
    }

    char nomefile[dim];
    int pid,status;
    do{
        printf ("Inserisci nome file di cui vuoi visualizzare le prime %d righe: ",numerorighe);
        scanf("%s",nomefile);
        if (strcmp(nomefile,"fine")!=0){ // verifico file
            int fd=open (nomefile,O_RDONLY);
            if (fd<0){
                if (errno==ENOENT)
                    printf ("Il file %s non esiste!\n",nomefile);
                else{
                    perror("Errore apertura file!\n");
                    exit(3);
                }
            }
            else{
                close (fd);

                pid =fork();
                if (pid<0){
                    perror("Errore creazione figlio!\n");
                    exit (4);
                }
                if (pid==0){ //FIGLIO
                    printf ("Visualisso le prime %d righe del file %s:\n",numerorighe,nomefile);
                    // comando head, alla fine di ogni exec mettere terminatore (char*)0 
                    execlp("head","head","-n",argv[1],nomefile,(char*)0); // se ha successo esce dal file
                    perror("Errore nella exec!\n"); //se exec da errore, torna qui e perror scrive nello stderr (collegato in terminale), dicendo codice errore
                    exit (5);
                }
                else wait(&status); // //La funzione wait() è usata nei processi padre per aspettare la terminazione di un processo figlio.
                //Salva lo stato  del figlio di uscita in status
            }
        }
    } while(strcmp(nomefile,"fine")!=0);
}

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#define dim 1024

int main (int argc, char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: numero argomenti passati errato!\n");
        exit(1);
    }

    int numerorighe=atoi(argv[1]);
    if (numerorighe<=0){
        perror("Errore: argomento passato non valido!\n");
        exit (2);
    }

    char nomefile[dim];
    int fd, status;
    do {
        printf ("Inserisci nome file di cui visualizzre le prime %d righe(fine per teminare): ",numerorighe);
        scanf("%s",nomefile);

        fd=open(nomefile,O_RDONLY);
        if (fd<0)
            if(errno==ENOENT && strcmp(nomefile,"fine")!=0)
                printf ("Il file %s non esiste!\n",nomefile);
            else {
                perror("Errore apertura file!\n");
                exit (3);
            }
        else {
            close (fd);
            int pid=fork();
            if (pid <0){
                perror ("Errore creazione processo figliio!\n");
                exit (4);
            }

            if (pid==0){ // sono nel figlio
                printf ("Ateprima %d righe delfile: \n",numerorighe);
                // comando head, alla fine di ogni exec mettere terminatore (char*)0 
                execlp("head","head","-n",argv[1],nomefile,(char*)0); // se ha sucesso esce dal figlio
                perror("Errore nella exec\n"); //se exec da errore, torna qui e perror scrive nello stderr (collegato in terminale), dicendo codice errore
                exit (5);
            }
            else {
                wait (&status); //La funzione wait() è usata nei processi padre per aspettare la terminazione di un processo figlio.
                //Salva lo stato  del figlio di uscita in status
            }

        }
    }while(strcmp(nomefile,"fine")!=0);
}*/


