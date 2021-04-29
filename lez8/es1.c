#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

#define NUM_PROD 2
#define NUM_CONS 3
#define NUM_MSG 10

// utility macro
#define PTHREADCALL(r,c,e) \
    if((r=c)!=0) { errno=r;perror(e);exit(errno); }

#define data_size 128

typedef struct node {
    char * data;
    struct node  * next;
} node;

node * coda = NULL; //RISORSA CONDIVISA -- CODA FIFO 
pthread_mutex_t lock_coda = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void * produttore (void * arg);
void * consumatore (void * arg);
void insertNode (node ** list, char * data);
char * removeNode (node ** list);

int main (int argc, char * argv[]) {

    int M;//NUM PROD
    int N;//NUM CONS
    int K;//NUM MSG
    if (argc == 4) {
        M = atoi(argv[1]);
        N = atoi(argv[2]);
        K = atoi(argv[3]);    
    }else{
        M = NUM_PROD;
        N = NUM_CONS;
        K = NUM_MSG;
    }

    int iter = K/M;
    
    pthread_t * array_thread = malloc ((N+M)*sizeof(pthread_t));
    int i;
    int err;
    for (i=0;i<M;i++) {
        pthread_t t;
        PTHREADCALL(err,pthread_create(&t,NULL,&produttore,(void*)&iter),"Create");
        array_thread[i] = t;
    }
    printf ("Produttori creati...\n");
    fflush(stdout);
    for (i=0;i<N;i++) {
        pthread_t t;
        PTHREADCALL(err,pthread_create(&t,NULL,&consumatore,NULL),"Create");
        array_thread[i] = t;
    }
    printf ("Consumatori creati...\n");
    fflush(stdout);

    for (i=0;i<(N+M);i++) {
        PTHREADCALL(err,pthread_join(array_thread[i],NULL),"Join");
    }

    printf ("Fine...\n");
    return 0;
}

//INSERISCE K/M MESSAGGI NELLA CODA
void * produttore (void * arg) {
    int iter = *((int*)arg);
    int err;
    int i;
    for (i=0;i<iter;i++) {
        //PRENDO LOCK
        PTHREADCALL(err,pthread_mutex_lock(&lock_coda),"Lock");
        //AGGIUNGO MESSAGGIO
        char * msg = malloc(sizeof(data_size));
        sprintf(msg,"Produttore %d : %d\n",pthread_self(),i);
        insertNode(&coda,msg);
        //INVIO SIGNAL
        PTHREADCALL(err,pthread_cond_signal(&not_empty),"Signal");
        //RILASCIO LOCK
        pthread_mutex_unlock(&lock_coda);
    }

}

void * consumatore (void * arg) {
    //CICLO INFINITO

    //PRENDO LOCK

    //ASPETTO CONDIZIONE VERIFICATA 

    //LEGGO MESSAGGIO

    //SE FINE TERMINO 
    //ALTRIMENTI STAMPO 

    //RILASCIO LOCK 
}


//INSERIMENTO IN TESTA
void insertNode (node ** list, char * data) {
    node * new = malloc (sizeof(node));
    new->data = malloc(data_size*sizeof(char));
    new->data = data;
    new->next = *list;

    //INSERISCI IN TESTA
    *list = new;
}

//RIMOZIONE IN CODA 
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