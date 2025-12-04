#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>     

int main (int argc, char **argv){
    if (argc>3 || argc<1){
        fprintf (stderr,"Errore: numero di argomenti passato errato\n");
        exit (1);
    }

    int flag=0; //indica l'input (1=file specifizato, 0=stdin)
    char *opzione; // puntatore all'opzione -n
    char *nomefile; // file da cui leggo
    int n=10; // numero di righe che voglio visualizzare (se non indicato uso 10 di default)

    switch(argc){
        case 3: // ho ./programma -n nomefile
            opzione=argv[1];
            nomefile=argv[2];
            flag=1;

            if (opzione[0]!='-'){
                fprintf (stderr,"Errore: neccessario usare -[numero]\n");
                exit (2);
            }

            n=atoi(&opzione[1]); // converto il numero dopo - in un intero
            if(n <= 0) {
                fprintf(stderr, "Numero di righe non valido\n");
                exit(3);
            }

        break;
        case 2: // ho ./programma -n OPPURE ./programma nomefile
            opzione=argv[1];
            if (opzione[0]!='-'){
                nomefile=opzione;
                flag=1;
            } else n=atoi (&opzione[1]);
            if(n <= 0) {
                fprintf(stderr, "Numero di righe non valido\n");
                exit(3);
            }
        break;

        default:
        break;

    }

    // apertura file caso 3 o 2
    int fd;
    if (flag==1){
        if ((fd=open(nomefile,O_RDONLY))<0){
            perror("Errore apertura file input!\n");
            exit (4);
        }
    } else fd=STDIN_FILENO;

    int i=1; // conta le righe
    int nr; // numero byte letty
    char c; 
    while((nr=read (fd,&c,1))>0){
        if (i<n)
            write (0, &c,1);
        else exit(5);
        if (c=='\n')
            i++;
    }

    if (flag==1)
        close(fd);
}

