#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128
#define TABLE_SIZE 10

typedef enum{
    CREATE,
    UPDATE,
    DELETE
}OPCPDE;

typedef struct msg_body{
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}msg_body_t, table_field;

typedef struct sync_msg{
    OPCPDE op_code;
    msg_body_t msg_body;
}sync_msg_t;

int main(int argc, char *argv[])
{
    struct sockaddr_un name;
    int ret;
    int connection_socket;
    int data_socket;
    int data;
    char buffer[BUFFER_SIZE];

    // ------------ SERVER SETUP ---------- //
    unlink(SOCKET_NAME);
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("Master socket created\n");
 
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    ret = bind(connection_socket, (const struct sockaddr *) &name,
            sizeof(struct sockaddr_un));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("bind() call succeed\n");
    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // ------------ END SERVER SETUP ---------- //

    table_field serverTable[TABLE_SIZE];
    // Initial table content
    strcpy(serverTable[0].destination, "122.1.1.1");
    serverTable[0].mask = 32;
    strcpy(serverTable[0].gateway_ip, "10.1.1.1");
    strcpy(serverTable[0].oif, "Ethernet1");

    strcpy(serverTable[1].destination, "130.1.1.0");
    serverTable[1].mask = 24;
    strcpy(serverTable[2].gateway_ip, "10.1.1.1");
    strcpy(serverTable[3].oif, "Ethernet1");

    strcpy(serverTable[1].destination, "126.30.34.0");
    serverTable[1].mask = 24;
    strcpy(serverTable[2].gateway_ip, "20.1.1.1");
    strcpy(serverTable[3].oif, "Ethernet2");
    


    // INFINITE LOOP
    for (;;) {

        /* Wait for incoming connection. */
        printf("Waiting on accept() sys call\n");
        data_socket = accept(connection_socket, NULL, NULL);
        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        printf("Connection accepted from client\n");
        
        // Send current table
        printf("sending current table to new client\n");
        ret = write(data_socket, serverTable, sizeof(serverTable));
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
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
