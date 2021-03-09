#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000;
#define K1 100;
#define K2 120;  

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int main (int argc, char *argv[]) {
    long n = N;
    long k1 = K1;
    long k2 = K2;
    //printf ("n : %d, k1 : %d, k2 : %d",n,k1,k2);
    if (argc == 2) {
        if ((n = isNumber(argv[1])) == -1) {
            fprintf(stderr,"Usage : use %s <intero>\n",argv[0]);
            exit(-1);
        }
    } else if (argc == 4) {
        if ((n = isNumber(argv[1])) == -1 || (k1 = isNumber(argv[2])) == -1 || (k2 = isNumber(argv[3])) == -1) {
            fprintf(stderr,"Usage : use %s <intero> <intero> <intero>\n",argv[0]);
            exit(-1);
        }
    } else if (argc == 3 || argc > 4) {
        fprintf(stderr,"Usage : use %s [N] [K1 K2]\n",argv[0]);
        exit(-1);
    }

    //creo e inizializzo il contatore
    int * array_contatore = malloc((k2-k1)*sizeof(int));
    for (int i=0; i<(k2-k1); i++) {
        array_contatore[i] = 0;
    }
    //genero numeri random e incremento contatore
    unsigned int seed = time(NULL);
    for (int i=0; i<n; i++) {
        int n = rand_r(&seed) % (k2-k1);
        array_contatore[n] = array_contatore[n] + 1;
    }

    for (int i=0; i<(k2-k1); i++) {
        float perc = ((float)array_contatore[i]/n)*100;
        printf ("%ld : %.2f%%\n",i+k1,perc);
    }

    free(array_contatore);

    return 0;
}