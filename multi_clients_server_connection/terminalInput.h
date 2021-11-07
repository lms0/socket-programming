// Functions and structs related to terminal data processing

#ifndef TERMINAL_H_  
#define TERMINAL_H_

#include"table.h"
void handle_console_data(char* buffer, row** table, int* n_rows, int* monitored_fd_set);

#endif // TERMINAL_H_