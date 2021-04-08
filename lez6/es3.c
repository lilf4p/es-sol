#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char * argv []) {

    if (argc!=2) {
        printf("Usage %s N\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    pid_t pid;
    
    for (int i=0;i<n;i++) {
        if ((pid = fork())==-1) {
            perror("main : fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            printf ("Processo figlio : %d con padre : %d\n",getpid(),getppid());
            exit(0);
        }
    }

    sleep(20);
    printf("Termino il programma...");
    
    for (int i=0;i<n;i++) {
        if (waitpid(-1,NULL,0)==-1) {
            perror("main : waitpid");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}