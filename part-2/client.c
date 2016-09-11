#include <stdio.h>
#include "lib.h"
#define BUFFER_SIZE 1000

int main(int argc, char *argv[])
{
	int sd = UDP_Open(20000);
	struct sockaddr_in addr, addr2;
	int rc = UDP_FillSockAddr(&addr, "machine.cs.wisc.edu", 10000);
	char ack_message[BUFFER_SIZE];

	char message[BUFFER_SIZE];
        sprintf(message, "MSG from client");
	int timeout = 5; 

	rc = UDP_Timeout_Retry(sd, &addr, message, &addr2, ack_message, timeout, BUFFER_SIZE); 

	if (rc > 0)
	{
		printf("Ack message from server is: %s\n",ack_message );
	}
	return 0;
}
