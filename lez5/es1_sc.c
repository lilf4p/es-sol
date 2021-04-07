#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUF_SIZE 256

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int main (int argc, char * argv[]) {

    int bufsize = BUF_SIZE;

    if (argc > 4||argc < 3) {
        fprintf(stderr,"Usage : %s filein fileout [buffersize]\n",argv[0]);
        return -1;
    }
    if (argc == 4) {
        if ((bufsize = isNumber(argv[3]))==-1) {
            fprintf(stderr,"Usage : %s filein fileout [buffersize]\n",argv[0]);
            return -1;
        }
    }

    int fdi,fdo;
    char buf[bufsize];
    //APRO I FILE
    if ((fdi = open(argv[1],O_RDONLY)) == -1) {
        perror("Errore in apertura");
        exit(EXIT_FAILURE);
    }
    if ((fdo = open(argv[2],O_WRONLY|O_CREAT,0666)) == -1) {
        perror("Errore in apertura");
        exit(EXIT_FAILURE);
    }
    //LEGGO IL PRIMO FILE E SCRIVO NEL SECONDO
    int lr;
    int lw;
    while ((lr=read(fdi,buf,bufsize))>0) {
        lw = 0;
        if ((lw = write(fdo,buf,lr)) == -1) {
            perror("Errore: write");
            exit(EXIT_FAILURE);
        }
        if (lw < lr) {
            fprintf(stderr,"Write ha scritto meno byte di quelli che ho letto");
            return -1;
        }
    }
    if (lr==-1) {
        perror("Errore: read");
        exit(EXIT_FAILURE);
    }
    //CHIUDO I FILE 
    if (close(fdi) == -1) {
        perror("Errore: close");
        exit(EXIT_FAILURE);
    }
    if (close(fdo) == -1) {
        perror("Errore: close");
        exit(EXIT_FAILURE);
    }

    return 0;
}  