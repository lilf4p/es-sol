#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int main (int argc, char * argv[]) {

	char opt;
	int nfnd, mfnd, ofnd, hfnd;
	int narg, marg;
	char * oarg;

	nfnd = 0;
	mfnd = 0;
	ofnd = 0;
	hfnd = 0;
	narg = 0;
	marg = 0;
	oarg = NULL;

	while ((opt = getopt(argc,argv,"n:m:o:h")) != -1) {
		switch (opt) {
			case 'n': 
				if ((narg = isNumber(optarg)) == -1) {
					printf ("Error : use -n <num. intero>\n");
					return -1;
				}
				nfnd = 1;
				break;
			case 'm':
				if ((marg = isNumber(optarg)) == -1) {
					printf ("Error : use -m <num. intero>\n");
					return -1;
				}
				mfnd = 1;
				break;
			case 'o':
				oarg = optarg;
				ofnd = 1;
				break;
			case 'h':
				hfnd = 1;
				break;
			default: // '?'  //
				fprintf (stderr, "Usage: %s -n <num. intero> -m <num. intero> -o <stringa> -h",argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	//SE PRESENTE OPZIONE -h STAMPO SOLO LO USAGE MSG
	if (hfnd==1) {
		printf ("Usage : %s -n <num. intero> -m <num. intero> -o <stringa> -h\n",argv[0]);
		return 0;
	}

	//MANCA ARGOMENTO
	//LO CONTROLLO GIA' NELLO SWITCH DEFAULT
	/*
	if (optind>=argc) {
		fprintf (stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
	}
	*/

	//STAMPO LE OPZIONI CON GLI EVENTUALI ARGOMENTI
	if (nfnd)
	printf("-n: %d\n", narg);
    if (mfnd)
	printf("-m: %d\n", marg);
    if (ofnd)
	printf("-s: %s\n", oarg);

	exit (EXIT_SUCCESS);
}

