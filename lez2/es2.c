#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef int (*F_t) (const char*);

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int arg_n (const char* arg) {
	long num;
	if ((num = isNumber(arg)) == -1) {
		printf ("Error : use -n <num. intero>\n");
		return -1;
	} else {
		printf ("-n : %ld\n",num);
		return 0;
	}
}

int arg_m (const char* arg) {
	long num;
	if ((num = isNumber(arg)) == -1) {
		printf ("Error : use -m <num. intero>\n");
		return -1;
	} else {
		printf ("-m : %ld\n",num);
		return 0;
	}
}

int arg_o (const char* arg) {
	printf ("-o : %s\n",arg);
	return 0;
}

int main(int argc, char* argv[]) {
	
  	// controllo di argc ed inizializzazione del vettore V con i puntatori a funzione
	if (argc == 0) {
		fprintf (stderr, "Usage: %s -n <num. intero> -m <num. intero> -o <stringa> -h",argv[0]);
		exit(EXIT_FAILURE); 
	}
	
	F_t fun_n = arg_n;
	F_t fun_m = arg_m;
	F_t fun_o = arg_o;
	F_t V[4];
	V['n'%4] = arg_n;
	V['m'%4] = arg_m;
	V['o'%4] = arg_o; 

	int opt;
	while ((opt = getopt(argc,argv, "n:m:o:h")) != -1) {
    switch(opt) {
		case '?': { 
			fprintf (stderr, "Usage: %s -n <num. intero> -m <num. intero> -o <stringa> -h",argv[0]);
			exit(EXIT_FAILURE); 
		} break;
		case 'h': {
			printf ("Usage : %s -n <num. intero> -m <num. intero> -o <stringa> -h\n",argv[0]);
			break;
		}
		default:
			// invocazione della funzione di gestione passando come parametro l'argomento restituito da getopt
			if (V[opt%4]( (optarg==NULL ? argv[0] : optarg) ) == -1) {
				fprintf (stderr, "Usage: %s -n <num. intero> -m <num. intero> -o <stringa> -h",argv[0]);
				exit(EXIT_FAILURE);
			}
    }
  }
  return 0; 
} 