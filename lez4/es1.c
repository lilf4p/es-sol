#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char * argv[]) {

    if (argc!=2) {
        fprintf(stderr,"Usage : %s fileOutput\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    //PREPARO FILE 
    FILE * inputFile;
    FILE * outputFile;
    if ((inputFile = fopen("/etc/passwd","r"))==NULL) {
        perror ("Aprendo /etc/passwd");
        return -1;
    }
    //SE NON ESISTE LO CREA (SOLO SE APRO IN WRITE)
    outputFile = fopen(argv[1],"w");

    //LEGGO FILE FINO ALLA FINE
    int bufferLength = 255;
    char buffer[bufferLength];
    while (fgets(buffer,bufferLength,inputFile)!=NULL) {
        char * token = strtok(buffer,":");
        fprintf(outputFile,"%s",token);
        fprintf(outputFile,"\n");
    }

    //CHIUDO I FILE 
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}