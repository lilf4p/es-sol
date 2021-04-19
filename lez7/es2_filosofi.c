#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

//NUMERO FILOSOFI
int NF;
//RISORSA CONDIVISA -- NUMERO DI FORCHETTE DISPONIBILI PER OGNI FILOSOFO
int * forchette;
//LOCK E VC
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void * filosofo (void * arg);
void prendiForchette (int i);
void rilasciaForchette (int i);
int destra (int i);
int sinistra (int i);

int main (int argc, char * argv[]) {

    if (argc<2) {
        fprintf (stderr,"Usage : %s <N > 5>\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    NF = atoi(argv[1]);
    if (NF<5) {
        fprintf (stderr,"Usage : %s <N > 5>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    //TODO : CHIEDI BUG
    //INIZIALIZZO ARRAY VAR CONDIZIONE -- UNA CODA PER OGNI FILOSOFO
    //SVEGLIO SOLO CHI HA LA CONDIZIONE VERIFICATA -- CON UNA SOLA CODA (V.C) SVEGLIEREI TUTTI
    int err;
    int i;
    //INIZIALMENTE TUTTI I FILOSOFI HANNO DUE FORCHETTE DISPONIBILI 
    forchette = malloc (NF*sizeof(int));
    for (i=0;i<NF;i++) {
        forchette[i] = 2;
    }

    pthread_t * array_thread = malloc (NF*sizeof(pthread_t));
    for (i=0;i<NF;i++) {
        pthread_t t;
        array_thread[i] = t;
        if ((err = pthread_create(&t,NULL,&filosofo,(void*)&i)) != 0) {
            errno = err;
            perror("Main : Create Thread\n");
        }
        printf("Creato Thread %d\n",i);
    }
    
    for (i=0;i<NF;i++) {
        if ((err = pthread_join(array_thread[i],NULL))!=0) {
            errno = err;
            printf("ERROR Main : Join Thread %d\n",i); //ERRORE JOIN THREAD 0 -> PERCHE'?????
            fflush(stdout);
        }
    }

    printf ("Cena finita!\n");

    return 0;
}

void * filosofo (void * arg) {
    int id = *((int*)arg);
    for (int i=0;i<100;i++) {

        //MEDITA
        printf ("Filosofo %d : Medita v%d...\n",id,i);
        fflush(stdout);
        int rand1 = rand() % (999999 + 1 - 99) + 99; 
        struct timespec t1;
        t1.tv_nsec = rand1;
        t1.tv_sec = 0;
        nanosleep(&t1,NULL);

        //PRENDI FORCHETTE
        prendiForchette(id);

        //MANGIA
        printf ("Filosofo %d : Mangia v%d...\n",id,i);
        fflush(stdout);
        int rand2 = rand() % (999999 + 1 - 99) + 99; 
        struct timespec t2;
        t2.tv_nsec = rand2;
        t2.tv_sec = 0;
        nanosleep(&t2,NULL);

        //RILASCIA FORCHETTE
        rilasciaForchette(id);

    }
    return (void*)0;
}

void prendiForchette (int i) {
    pthread_mutex_lock (&mtx);

    while (forchette[i]!=2) {
        pthread_cond_wait (&cond,&mtx);
    }

    forchette[sinistra(i)]--;
    forchette[destra(i)]--;

    pthread_mutex_unlock (&mtx);
}

void rilasciaForchette (int i) {
    pthread_mutex_lock (&mtx);

    forchette[sinistra(i)]++;
    forchette[destra(i)]++;
    
    if (forchette[sinistra(i)]==2 || forchette[destra(i)]==2) {
        pthread_cond_signal (&cond);
    }

    pthread_mutex_unlock (&mtx);
}

int destra (int i) {
    int ret;
    if (i==0) ret = NF-1;
    else ret = i-1;
    return ret;
}

int sinistra (int i) {
    int ret;
    ret = (i+1) % NF;
    return ret;
}

