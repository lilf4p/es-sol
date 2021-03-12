#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int confronta (int (*fun) (const void*, const void*, size_t),float * MTXT,float * MDAT,int size) {
	return memcmp(MTXT,MDAT,size);
}

int main (int argc, char * argv[]) {
	
	if (argc!=4) {
		fprintf (stderr,"Usage : %s mat_dump.txt mat_dump.dat N",argv[0]);
		return -1;
	}

	int N = atoi(argv[3]);
	FILE *ipftxt;
	FILE *ipfdat;
	//APRO I FILE IN LETTURA
	if ((ipftxt = fopen(argv[1],"r"))==NULL) {
		perror("opening mat_dump.txt");
		return -1;
	}
	if ((ipfdat = fopen(argv[2],"rb"))==NULL) {
		perror("opening mat_dump.dat");
		return -1;
	}
	
	//LEGGO LE DUE MATRICI 
	float *MTXT = malloc (N*N*sizeof(float));
	float *MDAT = malloc (N*N*sizeof(float));
	int i,j;
	float v1;
	
	//MTXT
    for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
			fscanf(ipftxt,"%f",&v1);
            MTXT[i*N+j] = v1;
        }
    }

	//MDAT
	float v2;
	for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
			fread (&v2,sizeof(float),1,ipfdat);
			MDAT[i*N+j] = v2;
        }
    }

	//CONFRONTA LE MATRICI E STAMPA RISULTATO
	int (*fun) (const void*, const void*, size_t);
	fun = memcmp;
	printf ("RISULTATO DEL CONFRONTO : %d\n",confronta(fun,MTXT,MDAT,(N*N)));

	/*
	//DEBUG - STAMPA MATRICE 
	for (i=0;i<N;i++) {
        for (j=0;j<N;j++) {
			printf ("%.2f ",MDAT[i*N+j]);
        }
		printf("\n");
    }
	*/

	return 0;
}