#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "icl_hash.h"

#define data_size 128
//#define buf_size 10

typedef struct node {
    char * data;
    struct node  * next;
} node;

FILE * f;

//RISORSE CONDIVISE -- BUFFER DI COMUNICAZIONE == LISTE DI STRING
node * buffer_righe = NULL;
node * buffer_parole = NULL;

//LOCK E CV
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notEmpty1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t notEmpty2 = PTHREAD_COND_INITIALIZER;

void * reader (void * arg);
void * tokenizer (void * arg);
void * printer (void * arg);
void * printer_hash (void * arg);
int insertBuf1 (char * data); //return -1 in caso di errore
int insertBuf2 (char * data);// idem 
int sizeList (node * list); 
void printList (node * list);
void insertNode (node ** list, char * data);
char * removeNode (node ** list);

int main (int argc, char * argv []) {

    if (argc != 2) {
        fprintf (stderr,"Usage %s file",argv[0]);
        exit(EXIT_FAILURE);
    } 

    if ((f = fopen (argv[1],"r")) == NULL) {
        perror ("Main : Error opening file");
        exit(EXIT_FAILURE);
    }

    pthread_t t1,t2,t3;
    int status1,status2,status3;

    pthread_create (&t1,NULL,&reader,NULL);
    pthread_create (&t2,NULL,&tokenizer,NULL);
    pthread_create (&t3,NULL,&printer,NULL);
    printf ("THREAD CREATI...\n");
    fflush(stdout);
    pthread_join (t1,(void*) &status1);
    pthread_join (t2,(void*) &status2);
    pthread_join (t3,(void*) &status3);

    printf ("Reader terminato con stato %d\nTokenizer terminato con stato %d\nPrinter terminato con stato %d\n",status1,status2,status3);

    return 0;
}

//legge il file per righe e le inserisce in fondo al primo buffer (buffer_righe)
void * reader (void * arg) {
    char line[data_size];
    while (fgets(line,data_size,f) != NULL) {
        //printf("READER : %s\n",line);
        //fflush (stdout);
        if (insertBuf1 (line) == -1) {
            exit(EXIT_FAILURE); //OPPURE GLI CONCEDO UN TOT DI TENTATIVI PRIMA DI TERMINARLO 
        }
        struct timespec t1;
        t1.tv_nsec = 9999999;
        t1.tv_sec = 0;
        nanosleep(&t1,NULL);
       
    }
    insertBuf1 ("-1");
    return (void*)0;
}

//prende una riga (la prima) dal primo buffer (buffer_righe), la tokenizza, e inserisce i token in fondo al secondo buffer 
void * tokenizer (void * arg) {

    while (1) {
        //printf("ITERAZIONI TOKENIZER\n");
        //fflush(stdout);
        int err;
        //PRELEVO RIGA DAL PRIMO BUFFER
        if ((err = pthread_mutex_lock (&mtx1)) != 0) {
            errno = err;
            perror("Tokenizer : lock1");
            continue;
        }
        //printf ("TOKENIZER HA LA LOCK\n");
        //fflush(stdout);
        //BUFFER VUOTO --> ASPETTO
        while (buffer_righe == NULL) {
            //printf ("TOKENIZER VA IN WAIT\n");
            //fflush(stdout);
            if ((err = pthread_cond_wait (&notEmpty1,&mtx1)) != 0) {
                errno = err;
                perror("Tokenizer : wait1");
            }
            //printf ("TOKENIZER SVEGLIATO\n");
            //fflush(stdout);
        }
        //C'E' ALMENO UNA RIGA NEL BUFFER
        //PRENDO IL PRIMO DELLA LISTA -- FIFO
        /*
        char * line = malloc(data_size*sizeof(char));
        line = buffer_righe->data;
        printf("TOKENIZER : %s\n",line);
        fflush(stdout);
        //LO RIMUOVO
        node * tmp;
        tmp = buffer_righe;
        buffer_righe = buffer_righe->next;
        free(tmp);
        */
        //PRENDO L'ULTIMO DELLA LISTA
        char * line = malloc(data_size*sizeof(char));
        line = removeNode(&buffer_righe);
        //if (buffer_righe == NULL) empty1 = 1;
        pthread_mutex_unlock (&mtx1);

        if (strcmp(line,"-1") == 0) break;

        //TOKENIZZO E INSERISCO NEL SECONDO BUFFER 
        char * token = strtok(line," ");
        while (token) {
            //printf ("TOKENIZER : Token = %s\n",token);
            //fflush(stdout);
            if (insertBuf2(token) == -1) {
                exit(EXIT_FAILURE);
            }
            token = strtok(NULL," ");
        }
    
        
    }
    insertBuf2 ("-1");
    return (void*)0;
}

//prende una parola (la prima) dal secondo buffer (buffer_parole) e la stampa
void * printer (void * arg) {

    while (1) {
        int err;
        if ((err = pthread_mutex_lock (&mtx2))!=0) {
            errno = err;
            perror("Printer : lock2");
            continue;
        }
        while (buffer_parole == NULL) {
            if ((err = pthread_cond_wait (&notEmpty2,&mtx2))!= 0) {
                errno = err;
                perror("Printer : wait2");
                continue;
            }
        }
        //C'E' ALMENO UNA PAROLA DA STAMPARE 
        /*
        word = buffer_parole->data;
        node * tmp;
        tmp = buffer_parole;
        buffer_parole = buffer_parole->next;
        free (tmp);
        */

        //PRENDO L'ULTIMO
        //PRENDO L'ULTIMO DELLA LISTA
        char * word = malloc(data_size*sizeof(char));
        word = removeNode(&buffer_parole);
        //if (buffer_parole==NULL) empty2 = 1;
        pthread_mutex_unlock(&mtx2);
        //printf("Buffer 2 : prelevato dato\n");
        //fflush(stdout);

        if (strcmp(word,"-1") == 0) break;

        printf ("%s\n",word);
        fflush (stdout);
    }
    return (void*)0;
}

//inserisce una riga in fondo al buffer buffer_righe
int insertBuf1 (char * data) {
    
    int err;
    if ((err = pthread_mutex_lock (&mtx1))!=0) {
        errno = err;
        perror("Reader : lock1");
        return -1;
    }

    //INSERISCI IN CODA -- FIFO
    /*
    node * last = buffer_righe;
    if (buffer_righe == NULL) {
        buffer_righe = new;
    } else {
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new;
    }
    */

    //INSERISCI IN TESTA
    insertNode(&buffer_righe,data);
    //MANDO SEGNALE SOLO SE DA VUOTA PASSA A NON VUOTA
    pthread_cond_signal (&notEmpty1);
    pthread_mutex_unlock (&mtx1);
    return 0;
}

//inserisce una parola in fondo al buffer buffer_parole
int insertBuf2 (char * data) {
    
    int err;
    if ((err = pthread_mutex_lock (&mtx2))!=0) {
        errno =err;
        perror("Tokenizer : lock2");
        return -1;
    }
    /*
    while (full2==1) {
        pthread_cond_wait (&notfull2,&mtx2);
    }
    */
    //INSERISCI IN CODA -- FIFO
    /*
    node * last = buffer_parole;
    if (buffer_parole == NULL) {
        buffer_parole = new;
    } else {
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new;
    }
    */
    //INSERISCI IN TESTA
    insertNode(&buffer_parole,data);
    //MANDO SEGNALE SOLO SE NON PIU' VUOTA
    pthread_cond_signal (&notEmpty2);
    pthread_mutex_unlock (&mtx2);
    return 0;
} 

int sizeList (node * list) {
    int size = 0;
    while (list!=NULL) {
        size++;
    }
    return size;
}

void printList (node * list) {
    while (list!=NULL) {
        printf ("%s -> ",list->data);
        list = list->next;
    }
    printf ("\n");
}

void insertNode (node ** list, char * data) {
    node * new = malloc (sizeof(node));
    new->data = malloc(data_size*sizeof(char));
    new->data = data;
    new->next = *list;

    //INSERISCI IN TESTA
    *list = new;
}

char * removeNode (node ** list) {
    char * line = malloc(data_size*sizeof(char));
    node * curr = *list;
    node * prev = NULL;
    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }
    line = curr->data;
    //printf("TOKENIZER : %s\n",line);
    //fflush(stdout);
    //LO RIMUOVO
    if (prev == NULL) {
        *list = NULL;
    }else{
        prev->next = NULL;
        free(curr);
    }
    return line;
}


//TODO : CHIEDI BUG USANDO LA TABELLA HASH
void * printer_hash (void * arg) {
    
    icl_hash_t * ht = NULL;
    ht = icl_hash_create(500,NULL,NULL);
    char stampa = 0;
    while (1) {
        int err;
        if ((err = pthread_mutex_lock (&mtx2))!=0) {
            errno = err;
            perror("Printer : lock2");
            continue;
        }
        while (buffer_parole == NULL) {
            if ((err = pthread_cond_wait (&notEmpty2,&mtx2))!= 0) {
                errno = err;
                perror("Printer : wait2");
                continue;
            }
        }
        //prendo l'ultimo
        char * word = malloc(data_size*sizeof(char));
        word = removeNode(&buffer_parole);
        //if (buffer_parole==NULL) empty2 = 1;
        pthread_mutex_unlock(&mtx2);
        //printf("Buffer 2 : prelevato dato\n");
        //fflush(stdout);

        if (strcmp(word,"-1") == 0) break;

        //INSERISCI OCCORRENZA IN TABELLA HASH 
        
        if (!icl_hash_find(ht,word)) {
            icl_hash_insert(ht, word, (void*)word);
            stampa = 1;
        } else {
            free(word);
        }
        
    }
    //STAMPA LE OCCORRENZE
    int k;
    icl_entry_t *entry;
    char *key, *value;

    if (ht && stampa) {
	    printf("Le parole individuate sono:\n");
	    printf("-----------------------------------------------\n");
	    icl_hash_foreach(ht, k, entry, key, value)
	    printf("%s ", value);
	    printf("\n-----------------------------------------------\n");
	    icl_hash_destroy(ht, NULL, free);
    }   

    return (void*)0;
}