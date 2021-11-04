#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct row{
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}row;

row* table; // server's table
int n_rows; // number of rows in server's table

// print current table
void printTable(){

    printf("%-25s%-20s%-10s%-20s%-1s\n", "Row", "Destination", "Mask", "IP_Gateway", "OIF");
    printf("%-25s%-20s%-10s%-20s%-1s\n", "===", "===", "===", "===", "===");

    int i;
    for(i = 0; i < n_rows; i++)
    {
      printf("%-25d%-20s%-10d%-20s%-1s\n", i, (table+i)->destination, (table+i)->mask, (table+i)->gateway_ip, (table+i)->oif); 
    }
}

int main ()
{

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
    printTable();


    return 0;
}