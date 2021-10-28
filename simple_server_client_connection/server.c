// Server side
// Send array of rows (a struct that contains rows of the asked table) to only one client, no multiplexing
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

int connection_socket; // server socket

// set up server: socket(), bind(), listen()
void setUpServer();

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

    setUpServer();

    // Example dummy table to send
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

        /* Wait for incoming connection. */
        printf("Waiting on accept() sys call\n");
        data_socket = accept(connection_socket, NULL, NULL);
        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from client\n");

        
        /* Send array, row by row */
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