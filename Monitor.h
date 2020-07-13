#include <sys/inotify.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct WatchMe{
    int id;
    char fullPath[NAME_MAX];
    
} Watcher;

int watchCount = 0;

static void displayInotifyEvent(int Fd,struct inotify_event *i,Watcher mywatchers[1000]){
    printf(" wd =%2d; ",i->wd);
    if (i->cookie > 0)
        printf("cookie = %4d; ",i->cookie);

    // change this block below to change output
    printf("mask = ");
    if (i->mask & IN_ACCESS) printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB) printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE) printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE) printf("IN_CREATE ");
    if (i->mask & IN_DELETE) printf("IN_DELETE");
    if (i->mask & IN_DELETE_SELF) printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED) printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR) printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY) printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO) printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN) printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW) printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT) printf("IN_UMOUNT ");

    if (i->len > 0)
    printf("    name = %s\n",i->name);
    
    if (i->mask & IN_CREATE && i->mask & IN_ISDIR){
        printf(":::::::::::HEY:::::::::::%d\n",mywatchers[i->wd].id - 1);
        printf("\nDIRECTORY %s CREATED\n",i->name);
        printf("\n\n%s\n\n",mywatchers[i->wd - 1].fullPath);
        //char* temp = NULL;
        char* temp = malloc(strlen(mywatchers[i->wd -1 ].fullPath) + i->len + 1);
        //snprintf(temp,strlen(mywatchers[i->wd - 1].fullPath) + i->len,"%s/%s",mywatchers[i->wd -1].fullPath, i->name);
        strcpy(temp,mywatchers[i->wd - 1].fullPath);
        strcat(temp,"/");
        strcat(temp,i->name);
        printf("=> path %s\n",mywatchers[i->wd - 1].fullPath);
        printf("=> final %s",temp);
        //printf("\n%d\n",strlen(mywatchers));
        int diditwork = inotify_add_watch(Fd,temp,IN_ALL_EVENTS);
        if (diditwork == -1) {
            printf("\n\nWATCHFAILED\n\n");
            perror("\nPERROR\n");
            printf("\n%d::::\n",watchCount);
            //free(temp);
        } else{
            //int leng = sizeof(struct WatchMe) / sizeof(mywatchers[0]);
            watchCount++;
            printf("\n%d::::\n",watchCount);
            mywatchers[watchCount -1 ].id = diditwork;
            strcpy(mywatchers[watchCount -1 ].fullPath,temp);
        }
        free(temp);

    }

    
     
}

//static void AddNewDirToWatch(struct inf)
static void AddNewDirToWatch(int watchDescriptor,int dirCount,int inotifyDescriptor,char **paths){
    for (int x = 0; x < number_of_dirs; x++){
        watchDescriptor = inotify_add_watch(inotifyDescriptor,paths[x],IN_ALL_EVENTS); // adds all the files listed on argv as watches
        if (watchDescriptor == -1){
            printf("Error Creating Inotify Watcher For %s Exiting!\n",paths[x]);
            //return 1;
            exit(1);
        }
        printf("\nWatching %s using Wd %d\n",paths[x],watchDescriptor);

    }
}