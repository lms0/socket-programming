// Server 
// Sends an array of rows (a struct that contains rows of the table) to multiple clients
// The table contains rows as a dynamic array

#include "fd_monitor.h" 
#include "table.h"
#include "terminalInput.h"

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128
#define MAX_CLIENT_SUPPORTED    32
int monitored_fd_set[MAX_CLIENT_SUPPORTED]; // global array that contains file descriptor numbers
int connection_socket; // server socket


void setUpServer();  // set up server: socket(), bind(), listen()

int n_rows; // number of rows in server's table
row* table; // server's table


int main(int argc, char *argv[])
{
    int ret;
    int data_socket;
    char buffer[BUFFER_SIZE]; // buffer for console input
    fd_set readfds;
    intitiaze_monitor_fd_set(monitored_fd_set,MAX_CLIENT_SUPPORTED);
    add_to_monitored_fd_set(0,monitored_fd_set,MAX_CLIENT_SUPPORTED); // add terminal file descriptor
    setUpServer();

    /*Add master socket to Monitored set of FDs*/
    add_to_monitored_fd_set(connection_socket,monitored_fd_set, MAX_CLIENT_SUPPORTED); // add master socket file descriptor

    
    int i; // for loop index
    // Message initialization
    mssg* m = malloc(sizeof(struct mssg));       // allocate memory for message to send

   
    printTable(table, n_rows);

    for (;;) 
    {
        refresh_fd_set(&readfds,monitored_fd_set,MAX_CLIENT_SUPPORTED); /*Copy the entire monitored FDs to readfds*/
        /* Wait for incoming connection. */
        printf("Waiting on select() sys call\n");
        ret = select(get_max_fd(monitored_fd_set,MAX_CLIENT_SUPPORTED) + 1, &readfds, NULL, NULL, NULL);
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

            add_to_monitored_fd_set(data_socket,monitored_fd_set,MAX_CLIENT_SUPPORTED);

            printf("n_rows is  %d\n", n_rows);
            /* Send complete array, row by row */
            for (i = 0; i < n_rows ; i++)
            {   
                printf("sending row number %d\n", i);
                m->op_code = 1; // create 
                m->body = *(table+i);
                ret = write(data_socket, m, sizeof(struct mssg)); // send complete message with op code too
                if (ret == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else if(FD_ISSET(0, &readfds)){
            memset(buffer, 0, BUFFER_SIZE);
            ret = read(0, buffer, BUFFER_SIZE);
            printf("Input read from console : %s\n", buffer);
            handle_console_data(buffer, &table, &n_rows, monitored_fd_set);
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










