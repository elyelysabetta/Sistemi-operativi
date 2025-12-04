#include <fcntl.h>   // per open(), creat()
#include <unistd.h>  // per read(), write(), close()
#include <stdio.h>   // per perror()
#include <stdlib.h>  // per exit()

#define PERM 0644    // permessi del file di output

int main(int argc, char **argv) {
    char fileinput[20] = "fileinp.txt";           // nome del file di input
    char fileoutput[40] = "fileout.txt";    // nome del file di output

    int infile, outfile;            // file descriptor
    int nread;                      // numero di byte letti
    char buffer[BUFSIZ];            // buffer per la lettura/scrittura

    // Apertura del file di input in sola lettura
    infile = open(fileinput, O_RDONLY);
    if (infile < 0) {
        perror("Errore apertura file di input");
        exit(1);
    }

    // Creazione del file di output con i permessi specificati
    outfile = creat(fileoutput, PERM);
    if (outfile < 0) {
        perror("Errore creazione file di output");
        close(infile);
        exit(2);
    }

    // Lettura e scrittura finché ci sono dati nel file di input
    while ((nread = read(infile, buffer, BUFSIZ)) > 0) {
        write(outfile, buffer, nread);
    }

    // Chiusura dei file
    close(infile);
    close(outfile);

    return 0;
}






