#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>



// print current table
void printTable(row* table, int n_rows){

    printf("%-25s%-20s%-10s%-20s%-1s\n", "Row", "Destination", "Mask", "IP_Gateway", "OIF");
    printf("%-25s%-20s%-10s%-20s%-1s\n", "===", "===", "===", "===", "===");

    int i;
    for(i = 0; i < n_rows; i++)
    {
      printf("%-25d%-20s%-10d%-20s%-1s\n", i, (table+i)->destination, (table+i)->mask, (table+i)->gateway_ip, (table+i)->oif); 
    }
}

mssg* addRowToTable(row** table, int n_rows, mssg* mReceived){
    *table = realloc(*table, (n_rows+1) * sizeof(struct row));
    (*table)[n_rows] = mReceived->body;
    return mReceived;
 
}



void updateRow(row* table, int rowToUpdate, row r){

    *(table + rowToUpdate) = r;   
}

void updateRowToAllClients(row* table, int pos, int* monitored_fd_set){

    int fd_pos = 2; // set initial fd set to 2, as the first two are terminal and master sockets, not clients
    mssg* m = malloc(sizeof(struct mssg));
    m->op_code = 2; // update 
    m->body = *(table+pos); 
    while (monitored_fd_set[fd_pos] != -1)
    {
        
        int ret = write(monitored_fd_set[fd_pos], m, sizeof(struct mssg)); 
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        fd_pos++;
    }

}

// returns row number on table that has a certain destination
int searchRowByDestination(row* table, int n_rows, char* destToFind){

    int pos = 0;
    for (pos = 0; pos < n_rows; pos++)
    {
        if (strcmp(table[pos].destination, destToFind) == 0)
        {
            return pos;
        }
    } 
    return -1;
}


void dumpRowToAllClients(row* table, int* monitored_fd_set, int n_rows){

    int fd_pos = 2; // set initial fd set to 2, as the first two are terminal and master sockets, not clients
    mssg* m = malloc(sizeof(struct mssg));
    m->op_code = 1; // create 
    m->body = *(table+(n_rows-1)); // last row
    while (monitored_fd_set[fd_pos] != -1)
    {
        
        int ret = write(monitored_fd_set[fd_pos], m, sizeof(struct mssg)); 
        if (ret == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        fd_pos++;
    }

}