#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define size_op 128

// utility macro
#define SYSCALL(c,e) \
    if(c==-1) { perror(e);exit(errno); }

int main (int argc, char * argv[]) {

    while (1) {
        
        char op[size_op];
        char result[size_op];
        printf ("> ");
        memset(op, '\0', size_op);
        if (fgets(op,size_op,stdin) == NULL) {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
        
        if (strncmp(op,"quit",4) == 0) {
            printf("Sto uscendo...\n");
            break;
        }
        if (strncmp(op,"exit",4) == 0) {
            printf("Sto uscendo...\n");
            break;
        }
        
        //CREAZIONE PIPE
        int ipfd[2];
        if (pipe(ipfd) == -1) {
            perror("Pipe in ");
            exit(EXIT_FAILURE);
        }
        int opfd[2];
        if (pipe(opfd) == -1) {
            perror("Pipe out");
            exit(EXIT_FAILURE);
        }
        
        //FORKO ED ESEGUO IL COMANDO BC
        pid_t pid;
        switch(pid = fork()) {
            case -1 : {
               perror("Fork");
               exit(EXIT_FAILURE); 
            }
            case 0 : {//FIGLIO --> ESEGUE bc -lq
                close(ipfd[1]);
                close(opfd[0]);
                if (dup2(ipfd[0],0) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                if (dup2(opfd[1],1) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                if (dup2(opfd[1],2) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }    
                execl("/usr/bin/bc","bc","-lq",NULL);
                perror("Figlio : Exec");
                return -1;
            }         
        }
        SYSCALL(close(ipfd[0]),"close");
        SYSCALL(close(opfd[1]),"close");
        SYSCALL(write(ipfd[1], (char*)op,strlen(op)),"write");
        SYSCALL(read(opfd[0], result, size_op),"read");
        SYSCALL(close(ipfd[1]),"close"); //CHIUDO STANDARD INPUT DI BC PER FARLO TERMINARE
        SYSCALL(wait(NULL),"wait");
        
        fprintf(stdout, "Operazione: %s", op);
	    fprintf(stdout, "Risultato : %s\n", result);
        
    }

    return 0;
}