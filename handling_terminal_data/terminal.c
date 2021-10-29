// Gets a string from console, parses it into tokens (delimited by space, comma, etc)
// Checks if the string has the command structure "COMMAND <IP> <MASK_NUMBER> <GATEWAY_IP> <OIF>"
// If so, saves it in a row struct

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // strtol

typedef struct row_t{
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];
}Row;

int main ()
{
  
  Row receivedRow; // struct to save the received data
  
  char str[100];    // string input by user
  char strCopy[100];// copied string
  int n_tokens = 0;
  char * pch; 
  while (n_tokens != 5)
  {
      
      printf("Please put your command such as: C <IP> <MASK_NUMBER> <GATEWAY_IP> <OIF>\n");
      fgets(str, 100, stdin);
      strcpy(strCopy ,str);
  
    n_tokens = 0; 
    // make sure the number of arguments is = 5
    pch = strtok (str," ,-");
    while (pch != NULL)
    {
        pch = strtok (NULL, " ,-");  
        n_tokens++;
    } 
  }
  

  pch = strtok (strCopy," ,-");
  printf("Operation type: %c\n", *pch);
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
  return 0;
}

