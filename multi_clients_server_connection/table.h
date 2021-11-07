// Functions and structs related to the ARP table

#ifndef TABLE_H_  
#define TABLE_H_

typedef struct row{   // row of table
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}row;

typedef enum{   // message type
    CREATE,
    UPDATE,
    DELETE
}OPCODE;

typedef struct mssg{ // message to send
    OPCODE op_code;
    row body;
}mssg;

void printTable(row* table, int n_rows); 
mssg* addRowToTable(row** table, int n_rows, mssg* mReceived); 
int searchRowByDestination(row* table, int n_rows, char* destToFind); 
void updateRow(row* table, int rowToUpdate, row r); 
void updateRowToAllClients(row* table, int pos, int* monitored_fd_set);
int searchRowByDestination(row* table, int n_rows, char* destToFind);
void dumpRowToAllClients(row* table, int* monitored_fd_set, int n_rows);


#endif // TABLE_H_