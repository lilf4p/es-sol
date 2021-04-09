#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char * argv []) {

    if (argc!=2) {
        printf("Usage %s N\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    int status;
    if ((pid = fork()) == -1) {
        printf ("main : fork");
        exit(EXIT_FAILURE);
    }
    if (pid) { //padre
        if (waitpid(pid,&status,0)==-1) {
            perror("main : wait");
            exit(EXIT_FAILURE);
        }
        printf("Processo %d, mio padre e' %d\n",pid,(int)getpid());
    }else{ //figlio
        execlp("/bin/sleep","sleep",argv[1],(char*)NULL);
        exit(10);
    }
    return 0;
}