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

    FILE *fdi, *fdo;
    //APRO I FILE
    if ((fdi = fopen(argv[1],"r")) == NULL) {
        perror("Errore in apertura");
        exit(EXIT_FAILURE);
    }
    if ((fdo = fopen(argv[2],"w")) == NULL) {
        perror("Errore in apertura");
        exit(EXIT_FAILURE);
    }
    //LEGGO IL PRIMO FILE E SCRIVO NEL SECONDO
    char buf[bufsize];
    while ((fgets(buf,bufsize+2,fdi))!=NULL) {
        fprintf(fdo,"%s",buf);
    }
    //CHIUDO I FILE 
    if (fclose(fdi)!=0) {
        perror("Errore: close");
        exit(EXIT_FAILURE);
    }
    if (fclose(fdo)!=0) {
        perror("Errore: close");
        exit(EXIT_FAILURE);
    }

    return 0;
}  