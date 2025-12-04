/*Questo programma conta quante volte una parola (argv[1]) appare 
in ciascuno dei file specificati (argv[2], argv[3], ecc.)*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char **argv){
    if (argc<3){
        fprintf (stderr, "Errore: numero argomenti passati errato!\n");
        exit (1);
    }

    int pid, status;
    for (int i=2;i<argc;i++){
        pid=fork();
        if (pid<0){
            perror ("Errore creazione figlio\n");
            exit (2);
        }
        if (pid==0){
            execlp("grep","grep","-c",argv[i],argv[1],(char*)0);
            perror("Errore nella exec\n");
            exit (3);
        } else wait(&status);
    }
    return 0;
}
