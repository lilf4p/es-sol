#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define N 10

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;
int buffer = 0;

void* consumer (void* arg);
void* producer (void* arg);

int main () {

    //struttura dati condivisa - buffer di una posizione 
    //int buffer = 0; // vuoto == 0 -- pieno == 1

    //creo e lancio i thread
    pthread_t t1, t2;
    int status1, status2;

    pthread_create(&t1,NULL,&producer,NULL);
    pthread_create(&t2,NULL,&consumer,NULL);
    //sleep(20);
    pthread_join(t1,(void*) &status1);
    pthread_join(t2,(void*) &status2);

    printf ("Thread Produttore terminato con status %d\nThread consumatore terminato con status %d\n",status1,status2);

    return 0;
}

void* consumer (void* arg) {
    int n;
    while (1) {
        pthread_mutex_lock(&mtx);
        while (buffer==0) {
            pthread_cond_wait(&notempty,&mtx);
            printf("Consumatore Svegliato!\n");
            fflush(stdout);
        }
        n = buffer;
        buffer = 0;
        printf("Consumatore : %d\n",n);
        fflush(stdout);
        pthread_cond_signal(&notfull);
        pthread_mutex_unlock(&mtx);
        if (n==N) break;
    }
    return (void*)0;
}

void* producer (void* arg) {
    for (int i=1;i<=N;i++) {
        pthread_mutex_lock(&mtx);
        while (buffer>0) {
            pthread_cond_wait(&notfull,&mtx);
            printf("Produttore svegliato!\n");
            fflush(stdout);
        }
        buffer = i;
        printf ("Produttore : %d\n",i);
        fflush(stdout);
        pthread_cond_signal(&notempty);
        pthread_mutex_unlock(&mtx);
    }
    return (void*)0;
}