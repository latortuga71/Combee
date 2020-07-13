/// first be able to get a list of directories
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#define MAX_PATHS 10000

/* 
struct FTW {
    int base;
    int level;
}
*/
// (test) ? (when true) : (when false)

static char *list_of_dirs[100000];
int number_of_dirs = 0;

static int display_info(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf){

    // this prints "f/d/sl" depth
	printf("%-3s %2d ", 
    (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
	(tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
	(tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
	(tflag == FTW_SLN) ? "sln" : "???",
	ftwbuf->level);
    // if error print --- FTW_NS error
	if (tflag == FTW_NS)
		printf("-------");
    // if no error print size of file/dir
	else
		printf("%7jd", (intmax_t) sb->st_size);

    // prints (file path) (offset of filename from basename) 
    printf("   %-40s\n",fpath);
	//printf("   %-40s %d %s\n",fpath, ftwbuf->base, fpath + ftwbuf->base);
    
    if (tflag == FTW_D || tflag == FTW_DP){
        printf("\n%s is a directory\n",fpath);
        strcpy(list_of_dirs[number_of_dirs++],fpath); 
    }
    
	return 0;           /* To tell nftw() to continue */
}

char **get_recursive_dirs(const char *path){
	int flags = FTW_DEPTH;
    int x;
    
    for (x = 0;x < 10000; x++) { 
        list_of_dirs[x] = malloc(sizeof(char) * NAME_MAX + 1);
    }
	//if (argc > 2 && strchr(argv[2], 'd') != NULL)
	//flags |= FTW_DEPTH;
	//if (argc > 2 && strchr(argv[2], 'p') != NULL)
	//	flags |= FTW_PHYS;
    flags |=FTW_PHYS;
    

    if (nftw(path,display_info,20,flags) == -1){
        perror("error nftw");
        exit(EXIT_FAILURE);
    }
    printf("%d\n",number_of_dirs);
    for (int i = 0; i < number_of_dirs; i++ ){
            printf("%d\n",i);
            printf("%s",list_of_dirs[i]);
    }
    return list_of_dirs;

    // free memory
    /*
    for (; x > 0; x--) { 
        //list_of_dirs[x] = malloc(sizeof(char) * NAME_MAX + 1);
        free(list_of_dirs[x]);
    }
    */
	//exit(EXIT_SUCCESS);
}

