// Server side
// Send array of rows (a struct that contains rows of the asked table) to multiplexing
// the arraw of rows is a dynamic array

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128
#define MAX_CLIENT_SUPPORTED    32

int monitored_fd_set[MAX_CLIENT_SUPPORTED];
int client_result[MAX_CLIENT_SUPPORTED] = {0};
int connection_socket; // server socket


void setUpServer();  // set up server: socket(), bind(), listen()
static void intitiaze_monitor_fd_set();
static void add_to_monitored_fd_set(int skt_fd);
static void remove_from_monitored_fd_set(int skt_fd);
static void refresh_fd_set(fd_set *fd_set_ptr);
static int get_max_fd();


typedef struct row{
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}row;

int main(int argc, char *argv[])
{
    int ret;
    int data_socket;
    char buffer[BUFFER_SIZE]; // buffer for console input
    fd_set readfds;
    intitiaze_monitor_fd_set();
    add_to_monitored_fd_set(0);
    setUpServer();

    /*Add master socket to Monitored set of FDs*/
    add_to_monitored_fd_set(connection_socket);

    // Table initialization: 3 rows
    int i;
    int n_rows = 3;
    row* table = malloc(n_rows * sizeof(struct row)); // allocate for 3 initial rows
    for(i = 0; i < n_rows; i++)
    {
      strcpy((table+i)->destination ,"122.0.0.0");
      (table+i)->mask = 1 + i;
      strcpy((table+i)->gateway_ip ,"1.0.0.0");
      strcpy((table+i)->oif ,"ETH");
    }

    for (;;) 
    {
        refresh_fd_set(&readfds); /*Copy the entire monitored FDs to readfds*/
        /* Wait for incoming connection. */
        printf("Waiting on select() sys call\n");
        ret = select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);
        if (ret == -1)
        {
            printf("Select returned -1, error, trying again to block ...");
            continue;
        }

        if(FD_ISSET(connection_socket, &readfds)){

            /*Data arrives on Master socket only when new client connects with the server (that is, 'connect' call is invoked on client side)*/
            printf("New connection recieved recvd, accept the connection\n");
            data_socket = accept(connection_socket, NULL, NULL);
            if (data_socket == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            printf("Connection accepted from client\n");

            add_to_monitored_fd_set(data_socket);

            /* Send complete array, row by row */
            for (i = 0; i < n_rows ; i++)
            {
                printf("sending new row to client\n");
                ret = write(data_socket, (table+i), sizeof(struct row));
                if (ret == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
            
            /* Close socket. */
            close(data_socket);
            remove_from_monitored_fd_set(data_socket);


        }
        else if(FD_ISSET(0, &readfds)){
            memset(buffer, 0, BUFFER_SIZE);
            ret = read(0, buffer, BUFFER_SIZE);
            printf("Input read from console : %s\n", buffer);
        }

       
    }

    /*close the master socket*/
    close(connection_socket);
    printf("connection closed..\n");

    /* Server should release resources before getting terminated.
     * Unlink the socket. */

    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}


void setUpServer(){

    /*In case the program exited inadvertently on the last run,
     *remove the socket.
     **/

    unlink(SOCKET_NAME);

    /* Create Master socket. */

    /*SOCK_DGRAM for Datagram based communication*/
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    printf("Master socket created\n");

    /*initialize*/
    struct sockaddr_un name;
    memset(&name, 0, sizeof(struct sockaddr_un));

    /*Specify the socket Cridentials*/
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    /* Bind socket to socket name.*/
    /* Purpose of bind() system call is that application() dictate the underlying 
     * operating system the criteria of recieving the data. Here, bind() system call
     * is telling the OS that if sender process sends the data destined to socket "/tmp/DemoSocket", 
     * then such data needs to be delivered to this server process (the server process)*/
    int ret = bind(connection_socket, (const struct sockaddr *) &name,
            sizeof(struct sockaddr_un));

    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("bind() call succeed\n");
    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     * */

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}



/*Remove all the FDs, if any, from the the array*/
static void
intitiaze_monitor_fd_set(){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++)
        monitored_fd_set[i] = -1;
}

/*Add a new FD to the monitored_fd_set array*/
static void
add_to_monitored_fd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = skt_fd;
        break;
    }
}

/*Remove the FD from monitored_fd_set array*/
static void
remove_from_monitored_fd_set(int skt_fd){

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
static void
refresh_fd_set(fd_set *fd_set_ptr){

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

static int
get_max_fd(){

    int i = 0;
    int max = -1;

    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }

    return max;
}