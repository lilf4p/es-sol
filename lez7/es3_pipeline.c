#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define data_size 512
#define buf_size 10

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
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t notfull1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t notfull2 = PTHREAD_COND_INITIALIZER;

int empty1 = 1;
int full1 = 0;
int empty2 = 1;
int full2 = 0;

void * reader (void * arg);
void * tokenizer (void * arg);
void * printer (void * arg);
void insertLast1 (char * data); 
void insertLast2 (char * data);
int sizeList (node * list); 

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

    pthread_create (&t3,NULL,&printer,NULL);
    pthread_create (&t2,NULL,&tokenizer,NULL);
    pthread_create (&t1,NULL,&reader,NULL);
    printf ("THREAD CREATI...\n");
    fflush(stdout);
    pthread_join (t1,(void*) &status1);
    pthread_join (t2,(void*) &status2);
    pthread_join (t3,(void*) &status3);

    printf ("Reader terminato con stato %d\nTokenizer terminato con stato %d\nPrinter terminato con stato %d\n",status1,status2,status3);

    return 0;
}

void * reader (void * arg) {
    char line[data_size];
    while (fgets(line,data_size,f) != NULL) {
        insertLast1 (line);
        printf("Buffer 1 : inserito dato\n");
        fflush (stdout);
    }
    insertLast1 ("-1");
    return (void*)0;
}

void * tokenizer (void * arg) {

    while (1) {
        //printf ("TOKENIZER AVVIATO\n");
        //fflush(stdout);
        char * line = malloc(data_size*sizeof(char));

        //PRELEVO RIGA DAL PRIMO BUFFER
        pthread_mutex_lock (&mtx1);
        printf ("TOKENIZER HA LA LOCK\n");
        fflush(stdout);
        //BUFFER VUOTO --> ASPETTO
        while (empty1 == 1) {
            printf ("TOKENIZER VA IN WAIT\n");
            fflush(stdout);
            pthread_cond_wait (&cond1,&mtx1);
            printf ("TOKENIZER SVEGLIATO\n");
            fflush(stdout);
        }
        //C'E' ALMENO UNA RIGA NEL BUFFER
        //PRENDO IL PRIMO DELLA LISTA -- FIFO
        line = buffer_righe->data;
        //LO RIMUOVO
        node * tmp;
        tmp = buffer_righe;
        buffer_righe = buffer_righe->next;
        free(tmp);
        full1=0;
        if (buffer_righe == NULL) empty1 = 1;
        pthread_cond_signal (&notfull1);
        pthread_mutex_unlock (&mtx1);
        printf("Buffer 1 : prelevato dato\n");
        fflush(stdout);

        if (strcmp(line,"-1") == 0) break;

        //TOKENIZZO E INSERISCO NEL SECONDO BUFFER 
        char * token = strtok(line," ");
        while (token) {
            insertLast2 (token);
            printf ("Buffer 2 : inserito dato\n");
            fflush(stdout);
            strtok(NULL," ");
        }
    }
    insertLast2 ("-1");
    return (void*)0;
}

void * printer (void * arg) {

    while (1) {
        char * word = malloc(data_size*sizeof(char));
        pthread_mutex_lock (&mtx2);
        while (empty2 == 1) {
            pthread_cond_wait (&cond2,&mtx2);
        }
        //C'E' ALMENO UNA PAROLA DA STAMPARE 
        word = buffer_parole->data;
        node * tmp;
        tmp = buffer_parole;
        buffer_parole = buffer_parole->next;
        free (tmp);
        full2=0;
        if (buffer_parole==NULL) empty2 = 1;
        pthread_cond_signal(&notfull2);
        pthread_mutex_unlock(&mtx2);
        printf("Buffer 2 : prelevato dato\n");
        fflush(stdout);

        if (strcmp(word,"-1") == 0) break;

        printf ("%s\n",word);
        fflush (stdout);
    }
    return (void*)0;
}

void insertLast1 (char * data) {
    node * new = malloc (sizeof(node));
    new->data = malloc(data_size*sizeof(char));
    new->data = data;
    new->next = NULL;
    pthread_mutex_lock (&mtx1);
    while (full1 == 1) {
        printf("Reader va in wait...\n");
        fflush(stdout);
        pthread_cond_wait (&notfull1,&mtx1);
    }
    //INSERISCI IN CODA -- FIFO
    node * last = buffer_righe;
    if (buffer_righe == NULL) {
        buffer_righe = new;
    } else {
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new;
    }
    empty1 = 0;
    if ( sizeList(buffer_righe) == buf_size ) full1=1;
    pthread_cond_signal (&cond1);
    pthread_mutex_unlock (&mtx1);
}

void insertLast2 (char * data) {
    node * new = malloc (sizeof(node));
    new->data = malloc(data_size*sizeof(char));
    new->data = data;
    new->next = NULL;
    pthread_mutex_lock (&mtx2);
    while (full2==1) {
        pthread_cond_wait (&notfull2,&mtx2);
    }
    //INSERISCI IN CODA -- FIFO
    node * last = buffer_parole;
    if (buffer_parole == NULL) {
        buffer_parole = new;
    } else {
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new;
    }
    empty2 = 0;
    if ( sizeList(buffer_parole)==buf_size ) full2=1;
    pthread_cond_signal (&cond2);
    pthread_mutex_unlock (&mtx2);
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