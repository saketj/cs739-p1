#include <stdio.h>
#include "lib.h"
#include<assert.h>
#define BUFFER_SIZE 1000
#include<unistd.h>
int main(int argc, char *argv[]) 
{
	double drop_out_perc;
	sscanf(argv[1], "%lf", &drop_out_perc);
	struct sockaddr_in s;
	int sd = UDP_Open(10000);
	assert(sd > -1);
	while (1) 
	{
		char buffer[BUFFER_SIZE];  
		int rc = UDP_Read_With_Drop(sd, &s, buffer, BUFFER_SIZE, drop_out_perc);
		if (rc > 0) 
		{
			char reply[BUFFER_SIZE];
			sprintf(reply, "ACK from server");
			rc = UDP_Write(sd, &s, reply, BUFFER_SIZE);
			printf("Msg from client: %s, Sent ACK!\n", buffer);
		}
	}
	return 0;
}
