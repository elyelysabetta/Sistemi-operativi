#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv){
    if (argc!=2){
        fprintf(stderr, "Errore: numero argomenti pasati errato");
        exit (1);
    }

    if (argv[1][0]!='-') {
        fprintf (stderr,"Errore: neccessario usare -[numero]\n");
        exit (2);
    }

    int n=atoi(&argv[1][1]); // converto il numero dopo - in un intero
    int i=1; //numero di righe
    int continua=1; 
    int nr;
    char c;

    // leggo un carattere alla volta dal stdinput
    while(continua==1 &&(nr=read(0,&c,1)!=0)){ 
        write(1,&c,1); // scrivo il carattere letto sul terminale (stdoutput)

        if (c=='\n'){
            i++;
            if (i>n)
                continua=0;
        }
    }

}


