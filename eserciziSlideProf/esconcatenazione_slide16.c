#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define PERM 0744


// Funzione se il secondo carattere è 's' e il penultimo una cifra
int pattern(char *s){
    if (s[1]=='s' && s[strlen(s)-2]>='0' && s[strlen(s)-2]<='9')
        return 1;
    return 0;
}

int main (int argc, char **argv){
    int fd;
    char stringa[40], answer[3];
    char eol='\n'; //end of line
    int pos=0; // posizione nel file

    if (argc<2){
        fprintf(stderr, "Errore: devi inserire il nome del file come primo argomento passato\n");
        exit (1);
    }

    printf("Nome file su cui inserire le stringhe: %s\n",argv[1]);

    // apertura file
    if ((fd=open(argv[1],O_WRONLY|O_APPEND))<0)
        if ((fd=creat(argv[1],PERM))<0){
            fprintf (stderr,"Errore apertura file input!\n");
            exit (2);
        }
    
    pos=lseek (fd,0,SEEK_END); // vado alla fine del file

    printf ("Il file contiene %d byte\n",pos);

    do{
        printf ("Inserisci stringa: ");
        scanf ("%s",stringa);
        if (pattern(stringa)==1){
            write(fd, stringa, strlen(stringa));
            write (fd,&eol,1);
        } else printf ("La stringa inserita non rispetta il pattern (NON verrà inserita nel file!)\n");

        printf ("Vuoi finire? (si/no): ");
        scanf ("%s",answer);
    } while (strcmp(answer,"no")==0);

    close (fd);
    printf ("Fine programma!\n");
    return 0;
}



