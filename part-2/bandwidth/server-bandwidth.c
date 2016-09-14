#include <stdio.h>
#include<assert.h>
#include<unistd.h>

#include "../lib.h"

#define BUFFER_SIZE 65507 // MAX UDP packet size
#define ACK_SIZE 4


int main(int argc, char *argv[])
{
	double drop_out_perc;
	sscanf(argv[1], "%lf", &drop_out_perc);
	struct sockaddr_in s;
	int sd = UDP_Open(10000);
	assert(sd > -1);

	char reply[ACK_SIZE];
	sprintf(reply, "ACK");
	char buffer[BUFFER_SIZE];

	while (1)
	{
		int rc = UDP_Read_With_Drop(sd, &s, buffer, BUFFER_SIZE, drop_out_perc);
		if (rc > 0)
		{
			rc = UDP_Write(sd, &s, reply, ACK_SIZE);			
		}
	}
	return 0;
}
