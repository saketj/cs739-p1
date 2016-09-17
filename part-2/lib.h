#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include<stdlib.h>
#include <stdbool.h>

int UDP_Open(int port)
{
	int sd;
	if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		return -1;
	}
	struct sockaddr_in myaddr;
	bzero(&myaddr, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(port);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *) &myaddr, sizeof(myaddr)) == -1)
	{
		close(sd);
		return -1;
	}
	return sd;
}

int UDP_FillSockAddr(struct sockaddr_in *addr, char *ip, int port)
{
	bzero(addr, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET; // host byte order
	addr->sin_port = htons(port); // short, network byte order
	/*struct in_addr *inAddr;
	struct hostent *hostEntry;
	if ((hostEntry = gethostbyname(hostName)) == NULL)
	{
		return -1;
	}
	//inAddr = (struct in_addr *) hostEntry->h_addr;
	//addr->sin_addr = *inAddr;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	*/

	if (inet_aton(ip, &addr->sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		return -1;
	}
	return 0;
}


int UDP_Write(int sd, struct sockaddr_in *addr, char *buffer, int n)
{
	int addrLen = sizeof(struct sockaddr_in);
	return sendto(sd, buffer, n, 0, (struct sockaddr *) addr, addrLen);
}

int UDP_Read(int sd, struct sockaddr_in *addr, char *buffer, int n)
{
	int len = sizeof(struct sockaddr_in);
	return recvfrom(sd, buffer, n, 0, (struct sockaddr *) addr,(socklen_t *) &len);
}

bool Decision_To_Drop(double probability)
{
    return rand() <  probability * ((double)RAND_MAX + 1.0);
}

int UDP_Read_With_Drop(int sd, struct sockaddr_in *addr, char *buffer, int n, double drop_out_perc)
{
	int len = sizeof(struct sockaddr_in);
        int rc= recvfrom(sd, buffer, n, 0, (struct sockaddr *) addr,(socklen_t *) &len);

	if (drop_out_perc == 0)
	{
		return rc;
	}
	//decides to drop -->
	if (Decision_To_Drop(drop_out_perc))
	{
		rc =0;
		printf("Dropping message from client\n");
	}
	return rc;
}

int UDP_Timeout_Retry(int sd, struct sockaddr_in *addr, char *message, struct sockaddr_in *addr2, char *ack_message,int timeout, int n)
{
	int recd_ack = 0;
	int rcw =0, rcr =0;
        while(recd_ack == 0)
        {
                rcw = UDP_Write(sd, addr, message, n);
                if (rcw > 0)
             	{
                        struct timeval tv;
                        tv.tv_sec = timeout;
                        tv.tv_usec = 0;

                        setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

                        rcr = UDP_Read(sd, addr2, ack_message, n);
                        if(rcr > 0)
                        {
                                recd_ack = 1;
                        }
                        else
                        {
                                printf("Timeout, sending again\n");

                        }
                }
        }
	return rcr;
}

