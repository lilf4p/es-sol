#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define UNIX_PATH_MAX 108 /* man 7 unix */ 
#define SOCKNAME "/tmp/mysock"
#define N 100

// utility macro
#define SYSCALL(c,e) \
    if(c==-1) { perror(e);exit(EXIT_FAILURE); }

int main (void) {

    int pid;
    int sfd;
    int cfd;
    char request[N];
    char result[N];
    struct sockaddr_un sa;
    strncpy(sa.sun_path,SOCKNAME,UNIX_PATH_MAX);
    sa.sun_family=AF_UNIX;

    int ip[2];
    int op[2];

    if ((sfd = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    //SYSCALL(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)),"setsockopt");
    SYSCALL(bind(sfd,(struct sockaddr *)&sa,sizeof(sa)),"Bind");
    SYSCALL(listen(sfd,SOMAXCONN),"Listen");
    int new_connection = 1;
    while(1) {

        SYSCALL(pipe(ip),"Pipe");
        SYSCALL(pipe(op),"Pipe");
        if (new_connection == 1) { 
            printf("Listen for client ...\n");
            if ((cfd = accept(sfd,NULL,0)) == -1) {
                perror("Accept");
                exit(EXIT_FAILURE);
            }
            printf ("Connection accepted from client!\n");
            SYSCALL(write(cfd,"Welcome to lilf4p server!",26),"Write Socket");
            new_connection = 0;
        }
        SYSCALL(read(cfd,request,N),"Read");
        printf ("From Client : %s\n",request);

        //SE RICEVI QUIT DAL CLIENT --> CHIUDI CONNESSIONE --> RIMETTITI IN ATTESA 
        if (strncmp(request,"quit",4) == 0) {
            close(cfd);
            new_connection = 1;
        }else{

            //LANCIO NUOVO PROCESSO CHE ESEGUE bc -lq
            switch (pid = fork()) {
                case -1 : {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                case 0 : {
                    //FIGLIO
                    SYSCALL(close(ip[1]),"Close");
                    SYSCALL(close(op[0]),"Close");

                    SYSCALL(dup2(ip[0],0),"Dup2");
                    SYSCALL(dup2(op[1],1),"Dup2");
                    SYSCALL(dup2(op[1],2),"Dup2");

                    execl("/usr/bin/bc","bc","-lq",NULL);
                    perror("Figlio : Exec");
                    return -1;
                    exit(EXIT_FAILURE);
                }
            }


            //PADRE
            //CHIUDO DESCRITTORI CHE NON USO
            SYSCALL(close(ip[0]),"close");
            SYSCALL(close(op[1]),"close");

            //SCRIVO SU PIPE A PROCESSO FIGLIO
            SYSCALL(write(ip[1], (char*)request,strlen(request)),"Write Pipe");
            //LEGGO DALLA PIPE
            SYSCALL(read(op[0], result, N),"Read");
            SYSCALL(close(ip[1]),"close"); //CHIUDO STANDARD INPUT DI BC PER FARLO TERMINARE
            SYSCALL(wait(NULL),"wait");
            //SCRIVO SU SOCKET AL CLIENT
            SYSCALL(write(cfd,result,sizeof(result)),"Write Socket");
        }
    }

    SYSCALL(close(sfd),"Close");
    remove("/tmp/mysock");

    return 0;
}