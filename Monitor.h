#include <sys/inotify.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "webhook.h"

/* 
mywatcher[i->wd - 1] used in prints because watchers start at 1 and not zero unfortunately
watcher struct is defined in this file along with globals used to house the user cmdline args
*/

typedef struct WatchMe{
    int id;
    char fullPath[NAME_MAX];
    
} Watcher;

int watchCount = 0;
int wflag = 0;
char *wvalue = NULL;

static void displayInotifyEvent(int Fd,struct inotify_event *i,Watcher mywatchers[MAX_PATHS]){
    if (i->len > 0){
        char *buff = malloc(sizeof(char) * 1024);
        time_t now;
        time(&now); 
        if (i->cookie > 0){
            printf("%s ::: WatcherDescriptor %d ::: Cookie ::: %4d ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,i->cookie,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"CookieNum ::: %4d\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,i->cookie,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        if (i->mask & IN_MOVE_SELF) 
            printf("%s ::: WatcherDescriptor %d ::: File Renamed ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
        if (i->mask & IN_MOVED_FROM){
            printf("%s ::: WatcherDescriptor %d ::: File Was Moved From ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"File Was Moved From\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        if (i->mask & IN_MOVED_TO){
            printf("%s ::: WatcherDescriptor %d ::: File Was Moved To ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"File Was Moved To\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        if (i->mask & IN_CREATE && i->mask & IN_ISDIR){
            printf("%s ::: WatcherDescriptor %d ::: Directory Was Created ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"Directory Was Created\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
            char* temp = malloc(strlen(mywatchers[i->wd -1 ].fullPath) + i->len + 1);
            strcpy(temp,mywatchers[i->wd - 1].fullPath);
            strcat(temp,"/");
            strcat(temp,i->name);
            int diditwork = inotify_add_watch(Fd,temp,IN_ALL_EVENTS);
            if (diditwork == -1) {
                printf("\n\nWATCHFAILED\n\n");
                perror("\nERROR\n");
                printf("\n%d::::\n",watchCount);
            } else{
                watchCount++;
                mywatchers[watchCount -1 ].id = diditwork;
                strcpy(mywatchers[watchCount -1 ].fullPath,temp);
            }
            free(temp);
        }
        //else if (i->mask & IN_ACCESS)
        //    printf("%s ::: WatcherDescriptor %d ::: File/Directory Was Acccessed %s/%s :::\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name); 
        else if (i->mask & IN_CREATE){
            printf("%s ::: WatcherDescriptor %d ::: File Was Created ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"File Was Created\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_ISDIR && i->mask & IN_DELETE){
            printf("%s ::: WatcherDescriptor %d ::: Directory Was Deleted ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"Directory Was Deleted\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_DELETE){
            printf("%s ::: WatcherDescriptor %d ::: File Was Deleted ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"File Was Deleted\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_MODIFY){
            printf("%s ::: WatcherDescriptor %d ::: File Was Modified ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"File Was Modified\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_OPEN){
            printf("%s ::: WatcherDescriptor %d ::: File Was Opened ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"File Was Opened\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_OPEN && i->mask & IN_ISDIR){
            printf("%s ::: WatcherDescriptor %d ::: Directory Was Opened ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"Directory Was Opened\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_CLOSE){
            printf("%s ::: WatcherDescriptor %d ::: File Was Closed ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"File Was Closed\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_CLOSE && i->mask & IN_ISDIR){
            printf("%s ::: WatcherDescriptor %d ::: Directory Was Closed ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"Directory Was Closed\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
        else if (i->mask & IN_ATTRIB){
            printf("%s ::: WatcherDescriptor %d ::: Attributes Were Modified ::: %s/%s\n",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
            if (wflag == 1){
                char *data = malloc(1024);
                snprintf(data,1024,"{\"Time\" : \"%s\",\"WatchDescriptor\" : \"%d\", \"Message\" : \"Attributes Were Modified\", \"File\" : \"%s/%s\"}\0",strtok(ctime(&now),"\n"),i->wd,mywatchers[i->wd -1 ].fullPath,i->name);
                post(wvalue,data);
                free(data);
            }
        }
    }
    else
        return;

}