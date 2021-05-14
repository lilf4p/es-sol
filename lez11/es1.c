#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define UNIX_PATH_MAX 108 /* man 7 unix */ 
#define SOCKNAME "/tmp/mysock"
#define N 100
#define MAX_CLIENT 50
// utility macro
#define SYSCALL(c,e) \
    if(c==-1) { perror(e);exit(EXIT_FAILURE); }
// utility macro pthread
#define SYSCALL_PTHREAD(e,c,s) \
    if((e=c)!=0) { errno=e;perror(s);exit(EXIT_FAILURE); }

typedef struct node {
    int data;
    struct node  * next;
} node;

node * coda = NULL; //RISORSA CONDIVISA -- CODA FIFO 
pthread_mutex_t lock_coda = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

volatile sig_atomic_t term = 0; //FLAG SETTATO DAL GESTORE DEI SEGNALI DI TERMINAZIONE

void * worker (void * arg);
void capitalizer (char * str);
void insertNode (node ** list, int data);
int removeNode (node ** list);
static void gestore_term (int signum);

int main (int argc, char * argv []) {

    //--------GESTIONE SEGNALI---------//
    struct sigaction s;
    sigset_t set;
    SYSCALL(sigfillset(&set),"sigfillset");
    SYSCALL(pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");
    memset(&s,0,sizeof(s));
    s.sa_handler = gestore_term;

    //installo gestore per segnali terminazione
    SYSCALL(sigaction(SIGINT,&s,NULL),"sigaction");
    SYSCALL(sigaction(SIGQUIT,&s,NULL),"sigaction");
    SYSCALL(sigaction(SIGTERM,&s,NULL),"sigaction");
    SYSCALL(sigaction(SIGHUP,&s,NULL),"sigaction");

    SYSCALL(sigemptyset(&set),"sigemptyset");
    SYSCALL(pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");
    //-----------------------------------------//

    if (argc < 2) {
        fprintf (stderr,"Usage: %s N\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    int nt = atoi(argv[1]); //DIMENSIONE POOL

    //------THREAD POOL--------//
    pthread_t * thread_pool = malloc(nt*sizeof(pthread_t));
    pthread_t t;
    int i;
    int err;
    for (i=0;i<nt;i++) {
        SYSCALL_PTHREAD(err,pthread_create(&t,NULL,worker,NULL),"create");
        thread_pool[i] = t;  
    }
    //------------------------//

    //------CREO SERVER--------//
    struct sockaddr_un sa;
    strncpy(sa.sun_path,SOCKNAME,UNIX_PATH_MAX);
    sa.sun_family=AF_UNIX;
    int sfd;
    if ((sfd = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    SYSCALL(bind(sfd,(struct sockaddr *)&sa,sizeof(sa)),"Bind");
    SYSCALL(listen(sfd,SOMAXCONN),"Listen");

    while (1) {

        int cfd;
        printf("Listen for clients...\n");
        
        if ((cfd = accept(sfd,NULL,0)) == -1) {
           if (term==1) break;
           else {
               perror("accept");
               exit(EXIT_FAILURE);
           }
        }
        
        printf ("Connection accepted from client!\n");
        SYSCALL(write(cfd,"Welcome to lilf4p server!",26),"Write Socket");

        //INSERISCO IL CLIENT NELLA CODA 
        insertNode(&coda,cfd);

    }

    printf("Closing server...\n");
    for (int i=0;i<nt;i++) {
        insertNode(&coda,-1);
    }
    close(sfd);
    remove("/tmp/mysock");

    return 0;
}

//GESTISCE TUTTE LE RICHIESTE DI UN CLIENT CONNESSO
//PRELEVA UN CLIENT DALLA CODA -> LO SERVE FINCHE' NON TERMINA -> NE PRENDE UN ALTRO 
void * worker (void * arg) {

    char request[N];
    int new = 1;
    int cfd;

    while (1) {
        //PRELEVA UN CLIENT DALLA CODA IF NEW
        if (new==1) {
            //PRELEVA DALLA CODA
            cfd = removeNode(&coda);
            if (cfd==-1) break;
            new=0;
        }
        //SERVO IL CLIENT
        SYSCALL(read(cfd,request,N),"Read");
        printf ("From Client : %s\n",request);

        if (strncmp(request,"quit",4) == 0) {
            close(cfd);
            new=1;
        }else{
            //TRASFORMA STRINGA TUTTA MAIUSCOLE
            capitalizer(request);
            //INVIA LA RISPOSTA AL CLIENT
            SYSCALL(write(cfd,request,sizeof(request)),"Write");
        }

    }
    close(cfd);
    printf("Closing worker\n");
    fflush(stdout);
    return 0;

}

void capitalizer (char * str) {
    int i=0;
    char ch;
    while(str[i]) {
        ch = str[i];
        str[i] = toupper(ch);
        i++;
    }
}

//INSERIMENTO IN TESTA
void insertNode (node ** list, int data) {
    int err;
    //PRENDO LOCK
    SYSCALL_PTHREAD(err,pthread_mutex_lock(&lock_coda),"Lock");
    node * new = malloc (sizeof(node));
    new->data = data;
    new->next = *list;

    //INSERISCI IN TESTA
    *list = new;
    //INVIO SIGNAL
    SYSCALL_PTHREAD(err,pthread_cond_signal(&not_empty),"Signal");
    //RILASCIO LOCK
    pthread_mutex_unlock(&lock_coda);
}

//RIMOZIONE IN CODA 
int removeNode (node ** list) {
    int err;
    //PRENDO LOCK
    SYSCALL_PTHREAD(err,pthread_mutex_lock(&lock_coda),"Lock");
    //ASPETTO CONDIZIONE VERIFICATA 
    while (coda==NULL) {
        pthread_cond_wait(&not_empty,&lock_coda);
        printf("Consumatore Svegliato\n");
        fflush(stdout);
    }
    int data;
    node * curr = *list;
    node * prev = NULL;
    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }
    data = curr->data;
    //LO RIMUOVO
    if (prev == NULL) {
        *list = NULL;
    }else{
        prev->next = NULL;
        free(curr);
    }
    //RILASCIO LOCK
    pthread_mutex_unlock(&lock_coda);
    return data;
}

static void gestore_term (int signum) {
    term = 1; 
}