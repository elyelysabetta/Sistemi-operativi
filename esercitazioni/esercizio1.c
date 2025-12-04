/* Il programma chiede all'utente di inserire stringhe e le scrive in un file (specificato come 
argomento) finché non viene digitato "fine". Se il file esiste già, lo sovrascrive.*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define perm 0644
#define dim 1024

int main (int argc, char **argv){
    if (argc!=2){
        fprintf (stderr,"Errore: numero argomenti errato!\n");
        exit (1);
    }

    // file descriptor del file passato come argomento 
    int fd=open(argv[1],O_WRONLY | O_TRUNC |O_CREAT, perm);  // apro file
    if (fd<0){
        perror("Errore apertura file!\n");
        exit (2);
    }

    char stringa[dim];
    do{
        printf ("Inserisci stringa (fine per terminare): ");
        scanf ("%s",stringa);
        if (strcmp(stringa,"fine")!=0){
            strcat(stringa, " ");
            write (fd,stringa,strlen(stringa));
        }

    } while (strcmp(stringa,"fine")!=0);
    close (fd);
    return 0;
}

