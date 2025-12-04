#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    struct stat info;

    if (stat("/Users/elyelisabetta/Desktop/operativic/eserciziSlideProf", &info) != 0) {
        perror("Errore stat");
        exit(1);
    }

    printf("Tipo e permessi      : %o\n", info.st_mode);
    printf("Numero inode         : %llu\n", info.st_ino);
    printf("Dispositivo          : %d\n", info.st_dev);
    printf("Numero di link       : %hu\n", info.st_nlink);
    printf("UID                  : %u\n", info.st_uid);
    printf("GID                  : %u\n", info.st_gid);
    printf("Dimensione file      : %lld byte\n", info.st_size);
    printf("Ultimo accesso       : %ld\n", info.st_atime);
    printf("Ultima modifica      : %ld\n", info.st_mtime);
    printf("Ultimo cambio stato  : %ld\n", info.st_ctime);
    return 0;
}
