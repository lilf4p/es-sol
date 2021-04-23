#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

//NUMERO FILOSOFI/FORCHETTE
int NF;
//ARRAY DI MUTEX -- UNA PER OGNI FORCHETTA 
pthread_mutex_t * mtx;

void * filosofo (void * arg);
void prendiForchette (int id);
void rilasciaForchette (int id);
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

    int err;
    int i;

    mtx = malloc (NF*sizeof(pthread_mutex_t));
    //INIZIALIZZO ARRAY DI MUTEX
    for (i=0;i<NF;i++) {
        if ((err = pthread_mutex_init(&(mtx[i]),NULL)) != 0) {
            errno = err;
            perror("Main : Initialize Mutex\n");
        }
    }

    pthread_t * array_thread = malloc (NF*sizeof(pthread_t));
    for (i=0;i<NF;i++) {
        pthread_t t;
        if ((err = pthread_create(&t,NULL,&filosofo,(void*)&i)) != 0) {
            errno = err;
            perror("Main : Create Thread\n");
        }
        array_thread[i] = t;
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

void prendiForchette (int id) {
    //TODO : GESTISCI ERRORI
    if (id%2 == 0) {

        //PRENDI PRIMA A SINISTRA
        pthread_mutex_lock(&mtx[sinistra(id)]);
        //CERCA DI PRENDERE A DESTRA 
        pthread_mutex_lock(&mtx[destra(id)]);

    } else {

        //PRENDI PRIMA A DESTRA 
        pthread_mutex_lock(&mtx[destra(id)]);
        //CERCA DI PRENDERE A SINISTRA 
        pthread_mutex_lock(&mtx[sinistra(id)]);

    }

}

void rilasciaForchette (int id) {
    //TODO : GESTISCI ERRORI
    pthread_mutex_unlock(&mtx[sinistra(id)]);
    pthread_mutex_unlock(&mtx[destra(id)]);
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
