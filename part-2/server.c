#include <stdio.h>
#include "lib.h"
#include<assert.h>
#define BUFFER_SIZE 1000
#include<unistd.h>
int main(int argc, char *argv[]) 
{
	int sd = UDP_Open(10000);
	assert(sd > -1);
	while (1) 
	{
		struct sockaddr_in s;
		char buffer[BUFFER_SIZE];
		//sleep(30);
		int drop_out_perc = 100;  
		int rc = UDP_Read_With_Drop(sd, &s, buffer, BUFFER_SIZE, drop_out_perc);
		if (rc > 0) 
		{
			printf("%s\n", buffer);
			char reply[BUFFER_SIZE];
			sprintf(reply, "ACK from server");
			rc = UDP_Write(sd, &s, reply, BUFFER_SIZE);
		}
	}
	return 0;
}
