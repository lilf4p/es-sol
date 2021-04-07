#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAXARG 512

static void print_status (pid_t pid, int status) {
    if ( pid != 0) printf("Process %d ",(int)pid); 
    if (WIFEXITED(status)) /* term normale */ printf("Exit value: %d",WEXITSTATUS(status)); 
    if (WIFSIGNALED(status)) /* segnale */ printf("Killed signal: %d",WTERMSIG(status));
    if (WCOREDUMP(status)) /* core file */ printf("-- core dumped");
    /*...*/
    if (WIFSTOPPED(status)) printf("stopped");
    printf("\n");
}

static void execute (int argc, char * argv []) {
    pid_t pid;
    int status;
    switch (pid = fork()) {
        case -1: {//PADRE ERRORE
            perror ("Cannot fork");
            break;
        }
        case 0: {//FIGLIO
            execvp (argv[0],argv);
            perror ("Cannot exec");
            exit (EXIT_FAILURE);
        }
        default: {//PADRE
            if (waitpid(pid,&status,0) == -1) {
                perror ("waitpid:");
                exit(EXIT_FAILURE);
            }  
            print_status(pid,status);
        }
    }
}

int cmdexit (int argc, char * argv  []) {
    if (argc==1 && (strcmp(argv[0],"exit") == 0) ) {
        return 0;
    }else{ 
        return 1;
    }
}

int read_cmd_line (int * argc, char * argv []) {
    char buffer[MAXARG];
    int nchar;
    int bufsize = MAXARG;
    fgets (buffer,bufsize,stdin);
    buffer[strlen(buffer)-1] = '\0';
    if (strlen(buffer) > MAXARG) return -1;
    char * s = strtok(buffer," "); 
    while (s) {
        argv[*argc] = s;
        (*argc)++;
        s = strtok(NULL," ");
    }
    return 0;
}

int main () { 

    int argc = 0;
    char ** argv;
    argv = (char**) malloc(MAXARG*sizeof(char*));

    while (1) {
        printf ("> ");
        if (read_cmd_line(&argc,argv) != -1) {
            if (cmdexit(argc,argv)==0) {
                exit(EXIT_SUCCESS);
            }
            argv[sizeof(argv-1)] = NULL;
            execute(argc,argv);
        }else{
            fprintf (stderr,"invalid command!\n");
        }
        for (int i=0;i<argc;i++) {
            argv[i] == NULL;
        }
        argc = 0;
    }
    return 1;
}