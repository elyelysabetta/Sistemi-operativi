#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SIZE 1024

int totRichieste;

void gestore () {
	printf("Arrivato ctrl-c, svolti %d servizi\n", totRichieste);
	exit(0);
}

int main(int argc,char **argv) {
	int status, pid1, pid2, pid3;
	char tipologia[SIZE];
	int p1p2[2], p2p3[2];
		
	signal(SIGINT, gestore);
	printf("Inserisci tipologia:\n");
	
	scanf("%s",tipologia);
	while(strcmp(tipologia, "esci")!=0){
		if(pipe(p1p2)<0){
			perror("pipe");
			exit(3);
		}
		
		pid1=fork();
		if(pid1<0){
			perror("fork");
			exit(4);
		}
		if(pid1==0) { //Figlio F1
			close(1);
			dup(p1p2[1]);
			close(p1p2[0]); 
			close(p1p2[1]);
			execl("/bin/grep","grep",tipologia,argv[1],(char *) 0);
			perror("grep1");
			exit(5);
		}
		close(p1p2[1]);
		if(pipe(p2p3)<0){
			perror("pipe");
			exit(3);
		}
		
		pid2=fork();
		if(pid2<0){
			perror("fork");
			exit(4);
		}
		
		if(pid2==0){ // Figlio F2
			close(0);
			dup(p1p2[0]);
			close(p1p2[0]);
			close(1);
			dup(p2p3[1]);
			close(p2p3[0]);
			close(p2p3[1]);
			execl("/bin/grep", "grep", "da pagare", (char *) 0);
			perror("grep2");
			exit(6);
		}
		close(p1p2[0]);
		close(p2p3[1]);
		
		pid3=fork();
		if(pid3<0){
			perror("fork");
			exit(4);
		}
		if(pid3==0){ // Figlio F3
			close(0);
			dup(p2p3[0]);
			close(p2p3[0]);
			execl("/bin/sort", "sort", "-r", "-n", (char *) 0);
                        perror("sort");
                        exit(7);
		}
		
		close(p2p3[0]);
		
		wait(&status);
		wait(&status);
		wait(&status);
		
		totRichieste++;
		
		printf("Inserisci tipologia:\n");
		scanf("%s",tipologia);
	}
	printf("Fine applicazione, svolti %d servizi\n", totRichieste);
	return 0;
}
