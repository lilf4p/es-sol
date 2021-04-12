#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* 
 * Calcola ricorsivamente il numero di Fibonacci dell'argomento 'n'.
 * La soluzione deve effettuare fork di processi con il vincolo che 
 * ogni processo esegua 'doFib' al più una volta.  
 * Se l'argomento doPrint e' 1 allora la funzione stampa il numero calcolato 
 * prima di passarlo al processo padre. 
 */
static void doFib(int n, int doPrint);

int main(int argc, char *argv[]) {
    // questo programma puo' calcolare i numeri di Fibonacci solo fino a 13.  
    const int NMAX=13;
    int arg;
    
    if(argc != 2){
	fprintf(stderr, "Usage: %s <num>\n", argv[0]);
	return EXIT_FAILURE;
    }
    arg = atoi(argv[1]);
    if(arg <= 0 || arg > NMAX){
	fprintf(stderr, "num deve essere compreso tra 1 e 13\n");
	return EXIT_FAILURE;
    }   
    doFib(arg, 1);
    return 0;
}

static void doFib(int n, int doPrint) {

    pid_t pid1;
    pid_t pid2;
    int status1;
    int status2;

    if (n < 2) { //CASO BASE
        if (doPrint) printf("%d\n",n);
        exit(n);
    } else {

        if ((pid1 = fork()) == 0) { //FIGLIO
            doFib(n-1,0);
        } else {
            if (pid1 == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if ((pid2 = fork()) == 0) { //FIGLIO
                doFib(n-2,0);
            } else { //PADRE
                if (pid2 == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }

                int f1,f2;

                if (waitpid(pid1,&status1,0)==-1) {
                    perror("Wait1");
                    exit(EXIT_FAILURE);
                }
                if (WIFEXITED(status1)) f1 = WEXITSTATUS(status1);

                if (waitpid(pid2,&status2,0)==-1) {
                    perror("Wait2");
                    exit(EXIT_FAILURE);
                }
                if (WIFEXITED(status2)) f2 = WEXITSTATUS(status2); 

                if (doPrint==1) printf("%d\n",f1+f2);
                return exit(f1+f2);
            }
        }
    }
}







