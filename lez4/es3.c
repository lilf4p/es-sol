#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main (int argc, char * argv[]) {


	//PARSING OPTIONI 
	char opt;
	int lfnd, wfnd;
	lfnd = 0;
	wfnd = 0;
	int wcount = 0;
	int lcount = 0;

	while ((opt = getopt(argc,argv,"lw")) != -1) {
		switch (opt) {
			case 'l': 
				lfnd = 1;
				break;
			case 'w':
				wfnd = 1;
				break;
			default: // '?'  //
				fprintf (stderr, "Usage: %s [-l -w] file1 [file2 file3 ….]",argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	//LEGGO FILE E ESEGUO LE OPTIONI SELEZIONATE
	int i = (lfnd+wfnd)+1; 

	while (i<argc) {
		//APRO FILE IN INPUT
		FILE *ipf;
		if ((ipf = fopen(argv[i],"r"))==NULL) {
        perror ("Aprendo file");
        return -1;
    	}
		//CONTO RIGHE E NEWLINE
		char c; 
		while ((c = fgetc(ipf))!=EOF) {
			if (c=='\n') {
				lcount++;
				wcount++;
			} else if (c==' '||c==','||c==':'||c==';') {
				wcount++;
			}
		}
		printf("File : %s\n",argv[i]);
		if (lfnd) printf("Numero di righe : %d\n",lcount);
		if (wfnd) printf("Numero di parole : %d\n",wcount);
		if (lfnd==0&&wfnd==0) {
			printf("Numero di righe : %d\n",lcount);
			printf("Numero di parole : %d\n",wcount);
		}
		lcount=0;
		wcount=0;
		i++;
	}

	return 0;
}