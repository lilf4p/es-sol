#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void strtoupper(const char* in, size_t len, char* out);

int main (int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("Errore : numero di argomenti diverso da 2\n");
        return -1;
    }
    
    const char* in = argv[1];
    size_t len = strtol(argv[2],NULL,10);
    char out [len];

    strtoupper (in,len,out);

    for (int i=0;i<len;i++) {
        printf("%c",out[i]);
    }
    printf("\n");

    return 0;
}

void strtoupper(const char* in, size_t len, char* out) {

    int i=0;

    while ((in[i]!='\0')||(i==len)) {

        out[i] = toupper(in[i]);
        i++;

    }
}
