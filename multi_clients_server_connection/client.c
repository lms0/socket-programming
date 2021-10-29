// Client side
// Receives data for a table, row by row

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int data_socket; // client socket

// message struct: row received from server, to be created in client's table
typedef struct row{
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}row;

row* table; // client's table, dynamic array of rows
int n_rows = 0; // current rows of client's table

void setUpClient();
void addRowToTable(row* rowReceived);
void printTable();


int main(int argc, char *argv[])
{
    setUpClient();
    int i, ret;
    
    /* Read new table. */
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
        addRowToTable(rowReceived);
        n_rows++; // increase number of rows present in the client's table
        //printTable();
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

// add row to client's table
void addRowToTable(row* rowReceived){
    table = realloc(table, (n_rows+1) * sizeof(struct row));
    table[n_rows] = *rowReceived;
    printf("added row number %d\n", n_rows);
    printf("destination: %s\n", (table+n_rows)->destination);
    printf("mask: %d\n", (table+n_rows)->mask);
    printf("ip_gateway: %s\n", (table+n_rows)->gateway_ip);
    printf("oif: %s\n", (table+n_rows)->oif);
    printf("\n");
}

// print current table
void printTable(){
    int i;
    for(i = 0; i < n_rows; i++)
    {
      printf("values of row %d:\n" , i);
      printf("destination: %s\n", (table+i)->destination);
      printf("mask: %d\n", (table+i)->mask);
      printf("ip_gateway: %s\n", (table+i)->gateway_ip);
      printf("oif: %s\n", (table+i)->oif);
      printf("\n");
      
    }
}