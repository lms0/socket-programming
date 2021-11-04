#include "fd_monitor.h"

/*Remove all the FDs, if any, from the the array*/
void intitiaze_monitor_fd_set(int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++)
        monitored_fd_set[i] = -1;
}

/*Add a new FD to the monitored_fd_set array*/
void add_to_monitored_fd_set(int skt_fd,int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = skt_fd;
        break;
    }
}

/*Remove the FD from monitored_fd_set array*/
void remove_from_monitored_fd_set(int skt_fd,int* monitored_fd_set,  const int MAX_CLIENT_SUPPORTED){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != skt_fd)
            continue;

        monitored_fd_set[i] = -1;
        break;
    }
}

/* Clone all the FDs in monitored_fd_set array into 
 * fd_set Data structure*/
void refresh_fd_set(fd_set *fd_set_ptr,int* monitored_fd_set,  const int MAX_CLIENT_SUPPORTED){

    FD_ZERO(fd_set_ptr);
    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] != -1){
            FD_SET(monitored_fd_set[i], fd_set_ptr);
        }
    }
}

/*Get the numerical max value among all FDs which server
 * is monitoring*/

int get_max_fd(int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED){

    int i = 0;
    int max = -1;

    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }

    return max;
}

void print_monitored_fd_set(int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED){
    int i;
    for (i = 0; i < MAX_CLIENT_SUPPORTED; i++ ){
        printf("%d, ", monitored_fd_set[i]);
    }
    printf("\n");
}