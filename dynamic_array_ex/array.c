#include<stdio.h>
#include<stdlib.h> //realloc()
#include<string.h>

typedef struct row{
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}row;

int main()
{
    int i;
    int n_rows = 3;
    row* table = malloc(n_rows * sizeof(struct row)); // allocate for 3 initial rows

    for(i = 0; i < n_rows; i++)
    {
      strcpy((table+i)->destination ,"122.0.0.0");
      (table+i)->mask = 1;
      strcpy((table+i)->gateway_ip ,"1.0.0.0");
      strcpy((table+i)->oif ,"ETH");
    }


    //realloc memory
    int new_n_rows = 5; 
    table = realloc(table, new_n_rows * sizeof(struct row));
    for(i=n_rows; i < new_n_rows; i++)
    {
      strcpy((table+i)->destination ,"155.0.0.0");
      (table+i)->mask = 2;
      strcpy((table+i)->gateway_ip ,"2.0.0.0");
      strcpy((table+i)->oif ,"ETH2");
    }
    n_rows = new_n_rows;
 

    //printing values
    for(i = 0; i < n_rows; i++)
    {
      printf("values of row %d:\n" , i);
      printf("destination: %s\n", (table+i)->destination);
      printf("mask: %d\n", (table+i)->mask);
      printf("ip_gateway: %s\n", (table+i)->gateway_ip);
      printf("oif: %s\n", (table+i)->oif);
      printf("\n\n");
      
    }

    free(table);

    return 0;
}