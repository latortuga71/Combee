#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#define MAX_PATHS 10000

static char *list_of_dirs[100000];
int number_of_dirs = 0;

static int display_info(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf){
    // if its a directory add it to the array
    if (tflag == FTW_D || tflag == FTW_DP){
        strcpy(list_of_dirs[number_of_dirs++],fpath); 
    }
	return 0;           /* To tell nftw() to continue */
}

char **get_recursive_dirs(const char *path){
	int flags = FTW_DEPTH;
    int x;
    flags |=FTW_PHYS;

    for (x = 0;x < MAX_PATHS; x++) { 
        list_of_dirs[x] = malloc(sizeof(char) * NAME_MAX + 1);
    }
    if (nftw(path,display_info,20,flags) == -1){
        perror("error nftw");
        exit(EXIT_FAILURE);
    }
    return list_of_dirs;

}

