#include <stdio.h>
#include <stdlib.h>

int somma (int x) {
    static int sum = INIT_VALUE; //DEFINITO A COMPILAZIONE -DINIT_VALUE=100;
    sum = sum+x;
    return sum;
}

int main (int argc, char *argv[]) {
    
    if (argc!=2) {
        fprintf (stderr,"Usage : %s n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int iter = atoi(argv[1]);

    for (int i=0;i<iter;i++) {
        int n;
        scanf("%d",&n);
        printf ("SOMMA PARZIALE %d: %d\n",i+1,somma(n));
    }

    return 0;
}
