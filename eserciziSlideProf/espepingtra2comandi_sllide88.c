#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int join(char *com1[], char *com2[]) {
    int status;
    int piped[2];
    pid_t pid1, pid2;

    if (pipe(piped) < 0) return 1;
    if ((pid1 = fork()) < 0) return 2;

    if (pid1 == 0) {
        close(piped[0]);
        dup2(piped[1], STDOUT_FILENO);
        close(piped[1]);
        execvp(com1[0], com1);
        exit(EXIT_FAILURE);
    }

    if ((pid2 = fork()) < 0) return 3;

    if (pid2 == 0) {
        close(piped[1]);
        dup2(piped[0], STDIN_FILENO);
        close(piped[0]);
        execvp(com2[0], com2);
        exit(EXIT_FAILURE);
    }

    close(piped[0]);
    close(piped[1]);
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    return WEXITSTATUS(status);
}

int main() {
    char *cmd1[] = {"ls", "-l", NULL};    // Primo comando
    char *cmd2[] = {"grep", "espe", NULL}; // Secondo comando
    
    printf("Esecuzione di: ls -l | grep espe\n");
    int result = join(cmd1, cmd2);
    
    printf("Completato con stato: %d\n", result);
    return 0;
}
