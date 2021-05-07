#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

// utility macro
#define SYSCALL(c,e) \
    if(c==-1) { perror(e);exit(EXIT_FAILURE); }

volatile sig_atomic_t nc = 0;
volatile sig_atomic_t nz = 0;
volatile sig_atomic_t flag = 0;

static void gestore (int signum) {
    if (signum == SIGINT) {
        write(1,"Ricevuto SIGINT\n",18);
        nc++;
    }
    if (signum == SIGTSTP) {
       write(1,"Ricevuto SIGSTP\n",18);
        flag=1;
        nz++; 
    }
}

static void gestore_sigstp (int signum) {
    write(1,"Ricevuto SIGSTP\n",18);
    flag=1;
    nz++;
}

int main () {

    struct sigaction s;
    sigset_t set;

    SYSCALL(sigfillset(&set),"sigfillset");
    SYSCALL(pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");

    memset(&s,0,sizeof(s));
    s.sa_handler = gestore;
    //installo gestore
    SYSCALL(sigaction(SIGINT,&s,NULL),"sigaction");
    SYSCALL(sigaction(SIGTSTP,&s,NULL),"sigaction");

    SYSCALL(sigemptyset(&set),"sigemptyset");
    SYSCALL(pthread_sigmask(SIG_SETMASK,&set,NULL),"pthread_sigmask");

    while (1) {
        if (flag) {
            char str[24];
            sprintf(str,"Numero di SIGINT : %d\n",nc);
            write(1,str,sizeof(str));
            flag = 0;
        }
        if (nz == 3) {
            printf("Terminare il processo? s/n \n");
            char c;
            alarm(10);
            scanf("%c",&c);
            if (c == 's') {
                break;
            }else{
                alarm(0);
            }
        }
    }

    return 0;
}