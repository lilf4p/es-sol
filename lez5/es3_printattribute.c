#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

void printAttribute (char * path) {
    struct stat info;
    if (stat(path,&info)==-1) {
        perror("stat");
    } else {
        printf ("NOME : %s\n",path);
        printf ("I-NODE : %ld\n",(long)info.st_ino);
        printf ("TIPO : ");
        if (S_ISREG(info.st_mode)) printf("-");
        else if (S_ISDIR(info.st_mode)) printf("d");
        else if (S_ISLNK(info.st_mode)) printf("l");
        else if (S_ISCHR(info.st_mode)) printf("c");
        else if (S_ISBLK(info.st_mode)) printf("b");
        else if (S_ISFIFO(info.st_mode)) printf("p");
        else if (S_ISSOCK(info.st_mode)) printf("s");
        else printf ("non riconosciuto");
        printf ("\nBIT DI PROTEZIONE : ");
        //USER
        if (S_IRUSR & info.st_mode) putchar('r');
        else putchar('-');
        if (S_IWUSR & info.st_mode) putchar('w'); 
        else putchar('-');
        if (S_IXUSR & info.st_mode) putchar('x'); 
        else putchar('-');
        //GROUP
        if (S_IRGRP & info.st_mode) putchar('r'); 
        else putchar('-');
        if (S_IWGRP & info.st_mode) putchar('w'); 
        else putchar('-');
        if (S_IXGRP & info.st_mode) putchar('x'); 
        else putchar('-');
        //ALTRI
        if (S_IROTH & info.st_mode) putchar('r'); 
        else putchar('-');
        if (S_IWOTH & info.st_mode) putchar('w'); 
        else putchar('-');
        if (S_IXOTH & info.st_mode) putchar('x'); 
        else putchar('-');
        printf ("\nUSER IDENTIFIER uid : %s\n", getpwuid(info.st_uid)->pw_name);
        printf ("GROUP IDENTIFIER gid : %s\n",getgrgid(info.st_gid)->gr_name);
        printf ("SIZE : %ld byte\n",info.st_size);
        printf ("ULTIMA MODIFICA : %s",ctime(&info.st_mtime));
    }
}

int main (int argc, char * argv[]) {

    if (argc<2) {
        fprintf (stderr,"Usage : %s file ...\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int i=1;
    while (i<argc) {
        printAttribute (argv[i]);
        printf("------------------------------\n");
        i++;
    } 
    return 0;
}