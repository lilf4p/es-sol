#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void print_ident (int k) {
    for (int i=0;i<k;i++) {
        printf("-");
    }
}

void fork_rec (int n) {
    pid_t pid;
    if (n==0) {
        print_ident (n+1);
        printf (" %d: sono l'ultimo discendente\n",getpid());
        exit(10);
    }
    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid) { //PADRE 
        print_ident (n+1);
        printf (" %d: creo un processo figlio\n",getpid());
        if (waitpid(pid,NULL,0) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        print_ident(n);
        printf(" %d: terminato con successo\n",pid);
    }else{ //FIGLIO
        fork_rec(n-1);
    }
}

int main (int argc, char * argv []) {

    if (argc!=2) {
        printf("Usage %s N\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    fork_rec(n);

    return 0;
}