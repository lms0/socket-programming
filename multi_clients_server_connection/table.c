#include "table.h"
#include <stdio.h>
#include <stdlib.h>




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

row* addRowToTable(row** table, int n_rows, row* rowReceived){
    *table = realloc(*table, (n_rows+1) * sizeof(struct row));
    (*table)[n_rows] = *rowReceived;
    return rowReceived;
}