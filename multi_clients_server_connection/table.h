// Functions and structs related to the ARP table

#ifndef TABLE_H_  
#define TABLE_H_

typedef struct row{       // row of ARP table
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}row;

void printTable(row* table, int n_rows); // show table
row* addRowToTable(row** table, int n_rows, row* rowReceived);  


#endif // TABLE_H_