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


int term = 0; //FLAG SETTATO DAL GESTORE DEI SEGNALI DI TERMINAZIONE
int sfd;

void * thread (void * arg);
void capitalizer (char * str);
void * gestore_term (void * arg);

int main (void) {

    int err;
    pthread_t t;
    pthread_attr_t attr;
    //MASCHERO SEGNALI CHE GESTISCO NEL THREAD A PARTE 
    int sig;
    sigset_t set;
    //MASCHERA CON SEGNALI DA GESTIRE
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGQUIT);
    sigaddset(&set,SIGTERM);
    sigaddset(&set,SIGHUP);
    //BLOCCO I SEGNALI CHE HO MASCHERATO -- LI ASPETTOP CON SIGWAIT
    SYSCALL_PTHREAD(err,pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");
    //AVVIO THREAD GESTIONE SEGNALI
    SYSCALL_PTHREAD(err,pthread_attr_init(&attr),"attr init");
    SYSCALL_PTHREAD(err,pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED),"attr detached");
    SYSCALL_PTHREAD(err,pthread_create(&t,&attr,&gestore_term,NULL),"create");

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
        //if (term==1) printf("FLAG SETTATO\n");break; //ANCHE SE GESTORE SETTA IL FLAG NON ESCE DAL WHILE, INCONTRA ACCEPT CHE DA ERRORE (SC INTERRUPT)
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

    //BLOCCO SEGNALI CHE GESTISCO CON THREAD A PARTE
    int sig;
    sigset_t set;
    int err;
    //MASCHERA CON SEGNALI DA GESTIRE
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGQUIT);
    sigaddset(&set,SIGTERM);
    sigaddset(&set,SIGHUP);
    //BLOCCO I SEGNALI CHE HO MASCHERATO -- LI ASPETTOP CON SIGWAIT
    SYSCALL_PTHREAD(err,pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");

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
    return (void*)0;
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

//SAREBBE PREFERIBILE SEGNALARE AL MAIN CHE DEVE TERMINARE E FARLO USCIRE NORMALMENTE
//UNA SOLUZIONE POTREBBE ESSERE IMPLEMENTARE LA COMUNICAZIONE CON UNA PIPE E USARE UNA SELECT NEL MAIN PER ASCOLTARE SIA PIPE CHE SOCKET CON ACCEPT
void * gestore_term (void * arg) {
    int err;
    int sig;
    sigset_t set;
    //MASCHERA CON SEGNALI DA GESTIRE
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGQUIT);
    sigaddset(&set,SIGTERM);
    sigaddset(&set,SIGHUP);
    //BLOCCO I SEGNALI CHE HO MASCHERATO -- LI ASPETTOP CON SIGWAIT
    SYSCALL_PTHREAD(err,pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");

    while (1) {

        sigwait(&set,&sig);

        if (sig == SIGINT || sig == SIGQUIT || sig == SIGTERM || sig == SIGHUP) {
            printf("Closing server...\n");
            close(sfd);
            remove("/tmp/mysock");
            exit(EXIT_SUCCESS);
        }

    }
    return (void*)0;
}