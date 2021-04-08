#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char * argv []) {

    if (argc!=2) {
        printf("Usage %s N\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    sleep(atoi(argv[1]));
    printf("Processo %d, mio padre e' %d\n",(int)getpid(),(int)getppid());

    return 0;
}