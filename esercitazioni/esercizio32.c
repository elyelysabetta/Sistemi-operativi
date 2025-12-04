/*Questo programma cerca quante volte una certa parola (nome oggetto) appare in più file specificati come argomenti.
Usa una gerarchia di processi (padre → figli → nipoti) e pipe per la comunicazione tra processi.
IL padre crea un figlio per ogni file passato come argomento e comunica con loro tramite pipe 
(pipein per inviare la parola da cercare, pipeout per ricevere i risultati).
Per ogni parola ricevuta dal padre, creano un nipote che esegue grep -c per contare le occorrenze 
nel file assegnato e inoltrano il risultato al padre tramite pipeout.
I nipoti eseguono grep -c "parola" file e restituiscono il conteggio al figlio.*/

// ./esercizio32 magazzino1.txt magazzino2.txt
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#define DIM 1024

typedef int pipee[2];

int main (int argc, char **argv){
    if (argc<2){
        fprintf(stderr,"Errore: numero argomenti passati errato");
        exit (1);
    }

    for (int i=1;i<argc;i++){
        // verifico esistenza file
        int fd=open(argv[i],O_RDONLY);
        if (fd<0){
            perror("Errore apertura file!\n");
            exit (2);
        }
        close (fd);

        //verifico path relativo 
        if (argv[i][0]=='/'){
            fprintf (stderr,"Errore: bisogna usare il path relativo!\n");
            exit (3);
        }
    }

    int numeroprocessi=argc-1;
    pipee pipein[numeroprocessi]; //da padre a figlio 
    pipee pipeout[numeroprocessi]; // da figlio a padre
    int pid,status;
    char nome[DIM],ret[DIM]; //nome da cercare e risultato della grep

    /* si genera 1 figlio per ogni file passato
    Figlio 1 (gestisce argv[1])-> Nipote 1 (esegue grep su argv[1])
    Figlio 2 (gestisce argv[2])-> Nipote 2 (esegue grep su argv[2]) */

    for (int i=0;i<numeroprocessi;i++){ // creo un figlio per ogni file passato
        if (pipe(pipein[i])<0){ // pipe padre scrive figlio legge
            perror("Errore pipein!\n");
            exit (4);
        }

        if (pipe(pipeout[i])<0){ //pipe figlio scrive padre legge
            perror ("Errore pipeout!\n");
            exit(5);
        }

        pid=fork();
        if (pid<0){
            perror ("Errore creazione figlio!\n");
            exit (6);
        }

        if (pid==0){ // FIGLIO
            for (int j=0;j<=i;j++){
                close(pipein[j][1]); // il figlio non legge da pipein[i][1], ma solo da pipein[i][0]
                close(pipeout[j][0]); // il figlio non legge da pipeout
            }

            while(read(pipein[i][0],nome,sizeof(nome))>0){
                pid=fork();
                if (pid<0){
                    perror("Errore creazione nipote!\n");
                    exit(6);
                }
                if (pid==0){ //NIPOTE
                    close (pipein[i][0]); // chiudo lettura perche ho già letto sopra nel while
                    close(1); // chiudo stdout
                    dup (pipeout[i][1]); // redirige stdout verso la pipe di output
                    close(pipeout[i][1]);

                    execlp("grep","grep","-c",nome,argv[i+1],(char*)0);
                    perror("Errore exec!\n");
                    exit(7);
                }

                wait(&status); // figlio attende nipote
            }

            close (pipein[i][0]);
            close(pipeout[i][1]);
            exit (0);
        }

        close (pipein[i][0]); // il padre non legge da pipein
        close (pipeout[i][1]); // il padre non scrive su pipeout
    }

    do{
        printf ("Inserisci nome oggetto che vuoi cercare(fine per terminare): ");
        scanf ("%s",nome);
        for (int i=0;i<numeroprocessi;i++)
            if (write(pipein[i][1],nome,strlen(nome)+1)<0){ //passo la materia che sto cercando ai figli
                perror("Errore nella write!\n");
                exit (8);
            }
        
        for (int i=0;i<numeroprocessi;i++){
            int nret;
            memset(ret,0,sizeof(ret)); //azzera il buffer
            //leggo dalla pipe il risultato della grep
            if ((nret=read (pipeout[i][0],ret, sizeof(ret)))<0){
                perror ("Errore nella read!\n");
                exit (9);
            }
            if (strcmp(nome,"fine")!=0){
                int numMateria=atoi(ret);
                printf ("Nel file %s ci sono %d unità di materia prima %s\n",argv[i+1],numMateria,nome);
            }
        }


    } while(strcmp(nome,"fine")!=0);

    for (int i=0;i<numeroprocessi;i++){
        close (pipein[i][1]);
        close(pipeout[i][0]);
        wait (&status);
    }
    return 0;
}

/*Padre crea pipein e pipeout per ogni file: scrive il nome da cercare (nome) su tutte le pipein e poi legge i risultati dalle pipeout
Figlio chiude i lati non usati delle pipe. Legge nome da pipein[i][0] e per ogni nome, forka un nipote (che esegue grep).
Nipote redirige stdout su pipeout[i][1] con dup e esegue grep -c nome file e scrive il risultato nella pipe */