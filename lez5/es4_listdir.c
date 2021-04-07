#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#define buf_size 512

void listDir (char * dirname);

int main (int argc, char * argv[]) {

    if (argc!=2) {
        fprintf(stderr,"Usage : %s dir\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    listDir (argv[1]);

    return 0;
}

void listDir (char * dirname) {

	DIR * dir;
	struct dirent* entry;

	if ((dir=opendir(dirname))==NULL) {
		return;
	}

    printf ("Directory: %s\n",dirname);
	while ((entry = readdir(dir))!=NULL) {
		char path[buf_size];
		snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name); 

        struct stat info;
        if (stat(path,&info)==-1) {
		perror("stat");
		exit(EXIT_FAILURE);
	    }

		//SE FILE E' UNA DIRECTORY 
		if (S_ISDIR(info.st_mode)) {
			if (strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0) continue;
			listDir(path);
		}

        //STAMPO INFO FILE
        printf ("\n%s\t%ld\t",path,info.st_size);
        //PERMESSI
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
        printf ("\n"); 
	}
    printf("----------------");

	if ((closedir(dir))==-1) {
		perror("closing directory");
		exit(EXIT_FAILURE);
	}
}
