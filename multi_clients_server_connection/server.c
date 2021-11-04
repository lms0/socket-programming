// Server side
// Send array of rows (a struct that contains rows of the ARP table) to multiple clients
// The table contains rows as a dynamic array

#include <stdlib.h> // strtol
#include "fd_monitor.h" 
#include "table.h"

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128
#define MAX_CLIENT_SUPPORTED    32
int monitored_fd_set[MAX_CLIENT_SUPPORTED]; // global array that contains file descriptor numbers
int connection_socket; // server socket


void setUpServer();  // set up server: socket(), bind(), listen()
void handle_console_data(char* buffer);
void dumpRowToAllClients();

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

    // Table initialization: 3 rows
    int i;
    n_rows = 3;
    table = malloc(n_rows * sizeof(struct row)); // allocate for 3 initial rows
    for(i = 0; i < n_rows; i++)
    {
      strcpy((table+i)->destination ,"122.0.0.0");
      (table+i)->mask = 1 + i;
      strcpy((table+i)->gateway_ip ,"1.0.0.0");
      strcpy((table+i)->oif ,"ETH");
    }
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
            print_monitored_fd_set(monitored_fd_set,MAX_CLIENT_SUPPORTED); // delete

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
            //close(data_socket);
            //remove_from_monitored_fd_set(data_socket);


        }
        else if(FD_ISSET(0, &readfds)){
            memset(buffer, 0, BUFFER_SIZE);
            ret = read(0, buffer, BUFFER_SIZE);
            printf("Input read from console : %s\n", buffer);
            handle_console_data(buffer);
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



void dumpRowToAllClients(){

    int fd_pos = 2; // set initial fd set to 2, as the first two are terminal and master sockets, not clients

    while (monitored_fd_set[fd_pos] != -1)
    {
        int ret = write(monitored_fd_set[fd_pos], (table+(n_rows-1)), sizeof(struct row)); // TODO: IMPLEMENT A DUMP() FOR ALL CONNECTED CLIENTS
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        fd_pos++;
    }

}

void handle_console_data(char* buffer){

    char commandType;
    row receivedRow;  // struct to save the received data
    char str[100];    // COPIED string input by user
    strcpy(str ,buffer);
    
    int n_tokens = 0;
    char * pch; 
     
    // make sure the number of arguments is = 5
    pch = strtok (str," ,-");
    while (pch != NULL)
    {
        pch = strtok (NULL, " ,-");  
        n_tokens++;
    } 
    printf("n_tokens = %d\n", n_tokens);
    if (n_tokens != 5)
    {
        printf("Could not read 5 tokens, please write it like: \n");
        printf("COMMAND_LETTER <IP> <MASK_NUMBER> <GATEWAY_IP> <OIF>\n");
    }
    else
    {
        pch = strtok (buffer," ,-");
        commandType = *pch;
        printf("Operation type: %c\n", commandType);
        pch = strtok (NULL, " ,-");
        strcpy(receivedRow.destination ,pch);
        pch = strtok (NULL, " ,-");
        receivedRow.mask = strtol(pch, NULL, 10);
        pch = strtok (NULL, " ,-");
        strcpy(receivedRow.gateway_ip ,pch);
        pch = strtok (NULL, " ,-");
        strcpy(receivedRow.oif ,pch);

        printf ("%s\n",receivedRow.destination);
        printf ("%d\n",receivedRow.mask);
        printf ("%s\n",receivedRow.gateway_ip);
        printf ("%s\n",receivedRow.oif);

        if (commandType == 'c' || commandType == 'C' )
        {
            printf("Row will be created\n");
            n_rows++;
            table = realloc(table, n_rows * sizeof(struct row));    
            strcpy((table+(n_rows-1))->destination ,receivedRow.destination);
            (table+(n_rows-1))->mask = receivedRow.mask;
            strcpy((table+(n_rows-1))->gateway_ip ,receivedRow.gateway_ip);
            strcpy((table+(n_rows-1))->oif ,receivedRow.oif);

            printTable(table,n_rows);

            printf("Sending new row to clients\n");
            /* Send (insert!) only last row */
            dumpRowToAllClients();
            
        }
        
    }
}



