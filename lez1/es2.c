#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
const int REALLOC_INC=16;
 
void RIALLOCA(char** buf, size_t newsize);

char* mystrcat(char *buf, size_t sz, char *first, ...);
 
int main(int argc, char *argv[]) {
	if (argc < 7) { printf("troppi pochi argomenti\n"); return -1; }
	char *buffer=NULL;
	RIALLOCA(&buffer, REALLOC_INC);  // macro che effettua l'allocazione del 'buffer'
	buffer[0]='\0';  //mi assicuro che il buffer contenga una stringa
	buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
	printf("%s\n", buffer); 
	free(buffer);
	return 0;
}

void RIALLOCA(char** buf, size_t newsize) {
  	//<inserire il codice per riallocare buf alla nuova size newsize>
	//printf("NELLA RIALLOCA\n");

	//RIALLOCA USANDO REALLOC
	//SE BUF==NULL --> REALLOC COME MALLOC(SIZE)
    char * tmp = realloc (*buf, newsize); 
    if (tmp==NULL) {
        puts ("Errore nella RIALLOCA\n");
        exit(1);
    } else {
        *buf = tmp;
		//printf("HO RIALLOCATO\n");
    }
}     

char* mystrcat(char *buf, size_t sz, char *first, ...) {
	//<implementare il codice che se necessario chiamerà RIALLOCA>

	char * buffer = buf;
	int size = sz;

	va_list ap;
	va_start (ap,first);

	char *str = va_arg(ap,char*);
	int j=0; //indice buffer == numero byte scritti 
	while (str!=NULL) {
		if (j + strlen(str)<size) { //se la stringa entra nel buffer
			//printf ("copio\n");
			//copio la stringa nel buffer (senza terminazione)
			for (int i=0;i<strlen(str) && str[i]!='\0';i++) {
				buffer[j] = str[i];
				j++;
			}
			//passo a nuova stringa 
			str = va_arg (ap,char*);
		} else { //incremento size buffer
			size = size + REALLOC_INC; //nuova size del buffer
			RIALLOCA(&buf,size);
			//printf("RIALLOCO\n");
		}
	}
	if (j+1<size) {
		buffer[j]='\0';
	} else {
		RIALLOCA(&buf,size+1);
		buffer[j]='\0';
	}
	va_end (ap);
	return buffer;
}  