// Client side
// Receives data for a table, row by row

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "table.h"

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int data_socket; // client socket

row* table = NULL; // client's table, dynamic array of rows
int n_rows = 0; // current rows of client's table

void setUpClient();



int main(int argc, char *argv[])
{
    setUpClient(); // socket(), connect()
    int i, ret;
    row* rowReceived = malloc(sizeof(struct row));
    
    for(;;)
    {   
        
        printf("Waiting for new row\n");
        ret = read(data_socket, rowReceived, sizeof(struct row));
        if (ret == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        
        if (ret == 0) // if server cut connection, break
        {
            printf("Server closed connection, Adieu!\n");
            break;
        }
        printf("About to add new row\n");
        rowReceived = addRowToTable(&table, n_rows, rowReceived);
        //addRowToTable(rowReceived);
        n_rows++; // increase number of rows present in the client's table
        printTable(table, n_rows);
    }

    /* Close socket. */
    //close(data_socket);
    //exit(EXIT_SUCCESS);
}

void setUpClient(){

    /* Create data socket. */

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (data_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * For portability clear the whole structure, since some
     * implementations have additional (nonstandard) fields in
     * the structure.
     * */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));

    /* Connect socket to socket address */

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    int ret = connect (data_socket, (const struct sockaddr *) &addr,
            sizeof(struct sockaddr_un));

    if (ret == -1) {
        fprintf(stderr, "The server is down.\n");
        exit(EXIT_FAILURE);
    }

    
}


