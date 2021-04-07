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

void findFile (char * dirname, char * fname);
int isDirectory (char * path);
void printLastModify (char * path);

int main (int argc, char *argv []) {
	
	if (argc!=3) {
		printf ("Usage : %s dir file\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	findFile (argv[1],argv[2]);

    return 0;
}

void findFile (char * dirname, char * fname) {

	DIR * dir;
	struct dirent* entry;

	if ((dir=opendir(dirname))==NULL) {
		return;
	}

	//CONTROLLO I FILE IN DIR
	while ((entry = readdir(dir))!=NULL) {
		char path[buf_size];
		snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

		//SE FILE E' QUELLO CHE CERCO
		if (strcmp(entry->d_name,fname)==0) {
			printf("DIR : %s",dirname);
			printf("				");
			printLastModify(path);
		}
		//SE FILE E' UNA DIRECTORY 
		if (isDirectory(path)) {
			if (strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0) continue;
			findFile(path,fname);
		}
	}
	if ((closedir(dir))==-1) {
		perror("closing directory");
		exit(EXIT_FAILURE);
	}
}

int isDirectory (char * path) {
	struct stat statbuf;
	if (stat(path,&statbuf)==-1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}
	return S_ISDIR(statbuf.st_mode);
}

void printLastModify (char * path) {
	struct stat statbuf;
	if (stat(path,&statbuf)==-1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}
	printf("ULTIMA MODIFICA : %s",ctime(&statbuf.st_mtime));
}