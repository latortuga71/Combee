#include <sys/inotify.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct WatchMe{
    int id;
    char fullPath[NAME_MAX];
    
} Watcher;

int watchCount = 0;
int wflag = 0;
char *cvalue = NULL;
int oflag = 0;
char *ovalue = NULL;

static void displayInotifyEvent(int Fd,struct inotify_event *i,Watcher mywatchers[MAX_PATHS]){
    // if webhook flag == 1

    if (i->len > 0){
        char *buff = malloc(sizeof(char) * 1024);
        time_t now;
        time(&now); 
        //printf("%s",ctime(&now));
        //printf("\n::: WatchDescriptor = %2d :::\n",i->wd);
        if (i->cookie > 0)
            printf("cookie = %4d; ",i->cookie);
        if (i->mask & IN_MOVE_SELF) printf("IN_MOVE_SELF %s\n",i->name);
        if (i->mask & IN_MOVED_FROM) printf("IN_MOVED_FROM %s\n",i->name);
        if (i->mask & IN_MOVED_TO) printf("IN_MOVED_TO %s\n",i->name);
        if (i->mask & IN_CREATE && i->mask & IN_ISDIR){
            printf("%s ::: WatcherDescriptor %d ::: Directory %s/%s CREATED :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            char* temp = malloc(strlen(mywatchers[i->wd -1 ].fullPath) + i->len + 1);
            strcpy(temp,mywatchers[i->wd - 1].fullPath);
            strcat(temp,"/");
            strcat(temp,i->name);
            //printf("=> path %s\n",mywatchers[i->wd - 1].fullPath);
            //printf("=> final %s",temp);
            int diditwork = inotify_add_watch(Fd,temp,IN_ALL_EVENTS);
            if (diditwork == -1) {
                printf("\n\nWATCHFAILED\n\n");
                perror("\nPERROR\n");
                printf("\n%d::::\n",watchCount);
                //free(temp);
            } else{
                watchCount++;
                //printf("\n%d::::\n",watchCount);
                mywatchers[watchCount -1 ].id = diditwork;
                strcpy(mywatchers[watchCount -1 ].fullPath,temp);
            }
            free(temp);
        }
        //else if (i->mask & IN_ACCESS)
        //    printf("%s ::: WatcherDescriptor %d ::: File/Directory Was Acccessed %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name); 
        else if (i->mask & IN_CREATE){
            printf("%s ::: WatcherDescriptor %d ::: File Was Created %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1)
                printf("CURL");
        }
        else if (i->mask & IN_ISDIR && i->mask & IN_DELETE)
            printf("%s ::: WatcherDescriptor %d ::: Directory Was Deleted %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        else if (i->mask & IN_DELETE)
            printf("%s ::: WatcherDescriptor %d ::: File Was Deleted %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        else if (i->mask & IN_MODIFY)
            printf("%s ::: WatcherDescriptor %d ::: File Was Modified %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        else if (i->mask & IN_OPEN)
            printf("%s ::: WatcherDescriptor %d ::: File Was Opened %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        else if (i->mask & IN_OPEN && i->mask & IN_ISDIR)
            printf("%s ::: WatcherDescriptor %d ::: Directory Was Opened %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        else if (i->mask & IN_CLOSE)
            printf("%s ::: WatcherDescriptor %d ::: File Was Closed %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        else if (i->mask & IN_CLOSE && i->mask & IN_ISDIR)
            printf("%s ::: WatcherDescriptor %d ::: Directory Was Closed %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        else if (i->mask & IN_ATTRIB)
            printf("%s ::: WatcherDescriptor %d ::: Attributes For %s/%s Were Modified:::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
    }
    else
        return NULL;

}