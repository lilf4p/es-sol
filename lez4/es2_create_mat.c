#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {

    if (argc!=2) {
        fprintf(stderr,"Usage %s N",argv[0]);
        return -1;
    }

    int i,j;
    int N = atoi(argv[1]);

    //INIZIALIZZO MATRICE
    float *M1 = malloc (N*N*sizeof(float));
    for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
            M1[i*N+j] = (i+j)/2.0;
        }
    }

    //CREO FILE TESTUALE(.TXT) E BINARIO(.DAT)
    FILE *opftxt = fopen ("mat_dump.txt","w");
    FILE *opfdat = fopen ("mat_dump.dat","wb");

    //SCRIVO SU FILE TXT
    for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
            fprintf(opftxt,"%.2f",M1[i*N+j]);
            fprintf(opftxt," ");
        }
    }

    //SCRIVO SU FILE DAT
    for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
            fwrite(&(M1[i*N+j]),sizeof(float),1,opfdat);
        }
    }

    //CHIUDO FILE
    fclose(opftxt);
    fclose(opfdat);

    return 0;
}