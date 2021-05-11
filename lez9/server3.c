#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define UNIX_PATH_MAX 108 /* man 7 unix */ 
#define SOCKNAME "/tmp/mysock"
#define N 100

// utility macro
#define SYSCALL(c,e) \
    if(c==-1) { perror(e);exit(EXIT_FAILURE); }

void capitalizer (char * str);
int updatemax(fd_set set, int fdmax);

int main (void) {

    int sfd; //WELCOME SOCKET --> ACCEPT
    int cfd; //SOCKET DI I/O CON UN CLIENT --> READ
    int num_fd = 0; //max file descriptor attivo
    int fd; //indice per scorrere i risultati della select
    char buf[N]; //buffer messaggio
    fd_set set; //insieme dei file descriptor attivi
    fd_set rdset; //insieme degli fd attesi in lettura
    
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

    //MANTENFO IL MAX INDICE DI DESCRITTORE ATTIVO IN NUM_FD
    if (sfd > num_fd) num_fd = sfd;
    FD_ZERO(&set);
    FD_SET(sfd,&set);

    printf("Listen for clients...\n");
    while (1) {
        rdset = set; //VA INIZIALIZZATO OGNI VOLTA PERCHE' SELECT LO MODIFICA
        SYSCALL(select(num_fd+1,&rdset,NULL,NULL,NULL),"select");
        for (fd=0;fd<=num_fd;fd++) {
            if (FD_ISSET(fd,&rdset)) {
                if (fd == sfd) { //WELCOME SOCKET PRONTO X READ
                    if ((cfd = accept(sfd,NULL,0)) == -1) {
                        perror("Accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(cfd,&set);
                    if (cfd > num_fd) num_fd = cfd;
                    printf ("Connection accepted from client!\n");
                    SYSCALL(write(cfd,"Welcome to lilf4p server!",26),"Write Socket"); 
                }else{ //SOCKET I/O CLIENT PRONTO X READ
                    SYSCALL(read(fd,buf,N),"Read");
                    printf ("From Client : %s\n",buf);

                    if (strncmp(buf,"quit",4) == 0) {
                        printf("Closing connection with client...\n");
                        FD_CLR(fd,&set);
                        if (fd == num_fd) num_fd = updatemax(set,num_fd);
                        close(fd);
                    }else{
                        //TRASFORMA STRINGA TUTTA MAIUSCOLE
                        capitalizer(buf);
                        //INVIA LA RISPOSTA AL CLIENT
                        SYSCALL(write(fd,buf,sizeof(buf)),"Write");
                    }
                }
            }
        }
    }
    close(sfd);
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

// ritorno l'indice massimo tra i descrittori attivi
int updatemax(fd_set set, int fdmax) {
    for(int i=(fdmax-1);i>=0;--i)
	if (FD_ISSET(i, &set)) return i;
    assert(1==0);
    return -1;
}