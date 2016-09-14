#include <stdio.h>
#include "lib.h"
#define BUFFER_SIZE 1000

int main(int argc, char *argv[])
{
	int timeout;
        sscanf(argv[1], "%d", &timeout);
	struct sockaddr_in addr, addr2;

	int sd = UDP_Open(20000);

	int rc = UDP_FillSockAddr(&addr, "machine.cs.wisc.edu", 10000);

	char message[BUFFER_SIZE];
        sprintf(message, "MSG from client");

	char ack_message[BUFFER_SIZE];
	rc = UDP_Timeout_Retry(sd, &addr, message, &addr2, ack_message, timeout, BUFFER_SIZE); 

	if (rc > 0)
	{
		printf("Ack message from server is: %s\n",ack_message );
	}
	return 0;
}
