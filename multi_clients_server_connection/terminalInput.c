#include"terminalInput.h"
#include"table.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // strtol


void handle_console_data(char* buffer, row** table, int* n, int* monitored_fd_set){

    printf("sent pointer to value n = %d\n", *n);
    int n_rows = *n;
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
    //printf("n_tokens = %d\n", n_tokens);
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

        if (commandType == 'c' || commandType == 'C' ) // if user wants to create a row
        {
            printf("Row will be created\n");
            n_rows++;
            *table = realloc(*table, n_rows * sizeof(struct row));    
            strcpy((*table+(n_rows-1))->destination ,receivedRow.destination);
            (*table+(n_rows-1))->mask = receivedRow.mask;
            strcpy((*table+(n_rows-1))->gateway_ip ,receivedRow.gateway_ip);
            strcpy((*table+(n_rows-1))->oif ,receivedRow.oif);

            printTable(*table,n_rows);

            printf("Sending new row to clients\n");
            /* Send (insert!) only last row */
            dumpRowToAllClients(*table, monitored_fd_set, n_rows);
            
        }
        else if (commandType == 'u' || commandType == 'U' ) // if user wants to update a row
        {
            printf("Row will be searched\n");
            int pos = searchRowByDestination(*table, n_rows, receivedRow.destination);
            if ( pos == -1)
            {
                printf("Row was not found\n");
            }
            else
            {
                printf("Updating row...\n");
                (*table+pos)->mask = receivedRow.mask;
                strcpy((*table+pos)->gateway_ip ,receivedRow.gateway_ip);
                strcpy((*table+pos)->oif ,receivedRow.oif);
                printTable(*table,n_rows);
                printf("Sending modified row to clients\n");
                updateRowToAllClients(*table, pos, monitored_fd_set);
            }
                

        }
        
    }
   
    *n = n_rows;
}
