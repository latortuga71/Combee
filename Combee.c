#include "GetDirs.h"
#include "Monitor.h"
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc,char *argv[]){
    // setting up variables
    int j;
    char buff[BUF_LEN] __attribute__((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;
    int inotifyFd;
    int wd;
    inotifyFd = inotify_init(); // returns FileDescriptor
    // get list of files to watch
    char** paths = get_recursive_dirs(argv[1]);

    Watcher mywatchers[10000];

    //printf("%s\n\n",paths[0]);
    //printf("%d\n",number_of_dirs);

    if (inotifyFd == -1){
        printf("Error Creating Inotify Instance! Exiting!\n");
        //return 1;
        exit(1);
    }
    
    for (int x = 0; x < number_of_dirs; x++){
        
        wd = inotify_add_watch(inotifyFd,paths[x],IN_ALL_EVENTS); // adds all the files listed on argv as watches
        mywatchers[x].id = wd;
        strcpy(mywatchers[x].fullPath,paths[x]);
        if (wd == -1){
            printf("Error Creating Inotify Watcher For %s Exiting!\n",paths[x]);
            return 1;
            //exit(1);
        }
        watchCount++;
        printf("\nWatching %s using Wd %d\n",paths[x],wd);

    } 
    
    //printf("\n\n\n\n%d:::%s\n\n",mywatchers[0].id,mywatchers[0].fullPath);
        // 
    for (;;){  // infinte loop to read events forever
        numRead = read(inotifyFd,buff,BUF_LEN);
        if (numRead <= 0 ){
            printf("Error Reading From Inotify FD Exiting....\n");
            return 1;
            //exit(1);
        }
        //printf("Read %ld bytes from inotifiy fd \n",(long)numRead);
        /// process all events in buffer returned by read()
        for (p = buff; p < buff + numRead; ){
            event = (struct inotify_event *) p; //cast to struct ptr 
            displayInotifyEvent(inotifyFd,event,mywatchers);
            
            
    
            p += sizeof(struct inotify_event) + event->len;
        }
        //break;
    }
   
}