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

#define UNIX_PATH_MAX 108 /* man 7 unix */ 
#define SOCKNAME "/tmp/mysock"
#define N 100

// utility macro
#define SYSCALL(c,e) \
    if(c==-1) { perror(e);exit(EXIT_FAILURE); }

void * thread (void * arg);
void capitalizer (char * str);

int main (void) {

    int sfd;
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

        int cfd;
        printf("Listen for clients...\n");
        if ((cfd = accept(sfd,NULL,0)) == -1) {
            perror("Accept");
            exit(EXIT_FAILURE);
        }
        printf ("Connection accepted from client!\n");
        SYSCALL(write(cfd,"Welcome to lilf4p server!",26),"Write Socket");

        //LANCIO UN THREAD PER LA CONNESSIONE COL CLIENT
        pthread_t t;
        pthread_create(&t,NULL,&thread,(void*)&cfd);

    }

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