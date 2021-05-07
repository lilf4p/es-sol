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


volatile sig_atomic_t term = 0; //FLAG SETTATO DAL GESTORE DEI SEGNALI DI TERMINAZIONE
volatile sig_atomic_t sfd;

void * thread (void * arg);
void capitalizer (char * str);
static void gestore_term (int signum);

int main (void) {

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

    //ignoro SIGPIPE
    s.sa_handler = SIG_IGN;
    SYSCALL(sigaction(SIGPIPE,&s,NULL),"sigaction");

    SYSCALL(sigemptyset(&set),"sigemptyset");
    SYSCALL(pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");
    //-----------------------------------------//

    struct sockaddr_un sa;
    strncpy(sa.sun_path,SOCKNAME,UNIX_PATH_MAX);
    sa.sun_family=AF_UNIX;

    //CREO LISTEN SOCKET
    if ((sfd = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    SYSCALL(bind(sfd,(struct sockaddr *)&sa,sizeof(sa)),"Bind");
    SYSCALL(listen(sfd,SOMAXCONN),"Listen");
    
    while(1) {
        //if (term) break; //ANCHE SE GESTORE SETTA IL FLAG NON ESCE DAL WHILE, INCONTRA ACCEPT CHE DA ERRORE (SC INTERRUPT)
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

        //LANCIO UN THREAD PER LA CONNESSIONE COL CLIENT
        pthread_t t;
        pthread_attr_t attr;
        int err;
        //CREO I THREAD COME DETACHED, OK O ALTRA SOLUZIONE? SE CHIUDO SERVER PRIMA CHE I THREAD TERMININO?
        //COME FARE JOIN SE DEVO CONTINUARE A ACCETTARE CLIENT? POSSO?
        SYSCALL_PTHREAD(err,pthread_attr_init(&attr),"attr init");
        
        SYSCALL_PTHREAD(err,pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED),"attr detached");
            
        SYSCALL_PTHREAD(err,pthread_create(&t,&attr,&thread,(void*)&cfd),"create");
            
    }
    //NON LO RAGGIUNGE!!!
    printf("Closing server...\n");
    close(sfd);
    remove("/tmp/mysock");

    return 0;
}

//THREAD CHE GESTISCE CONNESSIONE CON IL CLIENT CON SOCKET CFD
void * thread (void * arg) {

    int cfd = *((int *)arg);
    char request[N];

    while (1) {
        SYSCALL(read(cfd,request,N),"Read");
        printf ("From Client : %s\n",request);

        if (strncmp(request,"quit",4) == 0) {
            close(cfd);
            break;
        }

        //TRASFORMA STRINGA TUTTA MAIUSCOLE
        capitalizer(request);
        //INVIA LA RISPOSTA AL CLIENT
        SYSCALL(write(cfd,request,sizeof(request)),"Write");

    }
    close(cfd);
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

static void gestore_term (int signum) {
    term = 1; //SOLUZIONE MIGLIORE SAREBBE SETTARE IL FLAG, ESCO DAL WHILE E FACCIO LA TYERMINAZIONE NEL MAIN, MA NON FUNZIONE :/ 
    //SE TERMINO IL SERVER PRIMA CHE TERMININO I CLIENT, COME CHIUDO I SOCKET DEI CLIENT?
    //remove("/tmp/mysock");
    //close(sfd);
    //_exit(EXIT_SUCCESS);
}