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
    printf("%s\n\n",paths[0]);
    printf("%d\n",number_of_dirs);

    if (inotifyFd == -1){
        printf("Error Creating Inotify Instance! Exiting!\n");
        //return 1;
        exit(1);
    }
    Watcher mywatchers[1000];
    for (int x = 0; x < number_of_dirs; x++){
        
        wd = inotify_add_watch(inotifyFd,paths[x],IN_ALL_EVENTS); // adds all the files listed on argv as watches
        mywatchers[x].id = wd;
        strcpy(mywatchers[x].fullPath,paths[x]);
        //printf("BLEU\n%d:::::%s\n",mywatchers[x].id,mywatchers[x].fullPath);
        if (wd == -1){
            printf("Error Creating Inotify Watcher For %s Exiting!\n",paths[x]);
            return 1;
            //exit(1);
        }
        watchCount++;
        printf("\nWatching %s using Wd %d\n",paths[x],wd);

    }  
    printf("\n\n\n\n%d:::%s\n\n",mywatchers[0].id,mywatchers[0].fullPath);
        // 
    for (;;){  // infinte loop to read events forever
        numRead = read(inotifyFd,buff,BUF_LEN);
        if (numRead <= 0 ){
            printf("Error Reading From Inotify FD Exiting....\n");
            return 1;
            //exit(1);
        }
        printf("Read %ld bytes from inotifiy fd \n",(long)numRead);
        /// process all events in buffer returned by read()
        for (p = buff; p < buff + numRead; ){
            event = (struct inotify_event *) p; //cast to struct ptr 
            displayInotifyEvent(inotifyFd,event,mywatchers);
            
            
    
            p += sizeof(struct inotify_event) + event->len;
        }
        //break;
    }


    // if no args
    /*
    if (argc < 2 || strcmp(argv[1],"--help" ) == 0){
        printf("Usage\n%s pathname... \n",argv[0]);
        exit(1);
    }
    */


    /*
    // we have counter aka how many times it got a dir from the counte global variable
    char **initial_paths = get_recursive_dirs("/home/latortuga/Desktop/HoneyPot/Combee/");
    for (int y = 0; y < number_of_dirs; y++){
        printf("\n%s\n",initial_paths[y]);
    }
    //printf("Total Dirs %d\n",number_of_dirs);
    //printf("Watch attempt\n");
    //monitor_dir(initial_paths,number_of_dirs);
    //printf("end monitors\n");
    */
   /*
    // free mem
    for (int x = 0; x < MAX_PATHS; x++){
        free(initial_paths[x]);
    }
    */
    
}