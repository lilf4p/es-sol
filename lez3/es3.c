#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define dimN 16 //righe
#define dimM  8 //colonne
#define CHECK_PTR_EXIT(M,S)			\
            if (M==NULL) {			\
				perror(S);			\
				exit(EXIT_FAILURE);	\
            }
#define ELEM(M,I,J) M[i*dimM+j]
#define PRINTMAT(M,R,C) 							\
			for(size_t i=0;i<dimN;++i) {			\
				for(size_t j=0;j<dimM;++j) {		\
	    			printf ("%zu ",ELEM(M,i,j));	\
				}									\
				printf ("\n");						\
			}						    		
    

int main() {
    long *M = malloc(dimN*dimM*sizeof(long));
    CHECK_PTR_EXIT(M, "malloc"); 
    for(size_t i=0;i<dimN;++i)
		for(size_t j=0;j<dimM;++j)			
	    	ELEM(M,i,j) = i+j;    
    
    PRINTMAT(M, dimN, dimM);
    free(M);
    return 0;
}