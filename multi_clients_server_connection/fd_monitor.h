// Functions related to file descriptors

#ifndef FD_H_  
#define DF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


void intitiaze_monitor_fd_set(int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED);
void add_to_monitored_fd_set(int skt_fd, int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED);
void remove_from_monitored_fd_set(int skt_fd, int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED);
void refresh_fd_set(fd_set *fd_set_ptr,int* monitored_fd_set,  const int MAX_CLIENT_SUPPORTED);
int get_max_fd(int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED);
void print_monitored_fd_set(int* monitored_fd_set, const int MAX_CLIENT_SUPPORTED);

#endif // FD_H_