#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define size_op 128

int main (int argc, char * argv[]) {

    while (1) {
        
        char * op[size_op];
        char * result[size_op];
        getline(&op,size_op,stdin);

        if (strcmp(op,"quit") == 0) break;
        if (strcmp(op,"exit") == 0) break;

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
                execlp("usr/bin/bc","usr/bin/bc","-lq",(char*)NULL);
                perror("Figlio : Exec");
                return -1;
            }
            default : { //PADRE --> SCRIVE IN INPUT DELLA PIPE E LEGGE IL RISULTATO
                //todo : gestisci errori
                close(ipfd[0]);
                close(opfd[1]);
                write(ipfd[1], (char*)op,strlen(op));
                read(opfd[0], result, size_op);
                close(ipfd[1]); //CHIUDO STANDARD INPUT DI BC PER FARLO TERMINARE
                wait(NULL);
                return 1;
            }            
        }
        fprintf(stdout, "Operazione: %s", op);
	    fprintf(stdout, "Risultato : %s\n", result);

    }

    return 0;
}