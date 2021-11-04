// Simple signal handler
// When user sends the SIGINT signal from OS to process,
// by using ctrl C in the terminal,
// the app process it and adds a line of std out

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

//signal(SIGINT, handler);

static void handler(int sig);

int main()
{
        signal(SIGINT, handler);
	while(1){};
	return 0;
}

static void handler(int sig){

printf("Bye Bye!\n");
exit(0);
}
