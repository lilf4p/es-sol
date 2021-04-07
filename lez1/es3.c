#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int containHelp (int size, char *str[]) {
    for (int i=1;i<size;i++) {
        if (strcmp(str[i],"-h")==0) return 0;
    }
    return -1;
}

int main (int argc, char *argv[]) {
    int i=1;
    while (i<argc) {
        char *opt = argv[i];
        if (containHelp(argc,argv)==0) {
            printf ("nome-programma -n <numero> -s <stringa> -m <numero> -h\n");
            return 0;
        }
        if (opt[0]=='-') {
            //E' UN'OPZIONE 
            //PARSA OPZIONE
            if (opt[1]=='n') {
                //stampa numero
                if (opt[2]=='\0' && (i+1)<argc) {
                    char *arg = argv[i+1];
                    int n = isNumber(arg);
                    if (n!=-1) {
                        printf ("-n: %d ",n);
                        i=i+2;
                    } else {
                        printf ("Errore : usa -n <intero>\n");
                        return -1;
                    }
                } else {
                    i++;
                }
            } else if (opt[1]=='m') {
                //stampa numero
                if (opt[2]=='\0' && (i+1)<argc) {
                    char *arg = argv[i+1];
                    int n = isNumber(arg);
                    if (n!=-1) {
                        printf ("-n: %d ",n);
                        i=i+2;
                    } else {
                        printf ("Errore : usa -n <intero>\n");
                        return -1;
                    } 
                }else{
                    i++;
                }
            } else if (opt[1]=='s') {
                if ((i+1)<argc) {
                    printf ("-s: %s ",argv[i+1]);
                    i=i+2;
                }else{
                    i++;
                }
            } else {
                printf ("Errore :opzione non riconosciuta\n");
                return -1;
            }
        } else {
            //NON E' UN'OPZIONE
            printf ("Errore : usare -opzione <argomento>\n");
            return -1;
        }

    }

    return 0;
}