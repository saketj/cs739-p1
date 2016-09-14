#include <stdio.h>
#include <stdint.h>	/* for uint64 definition */
#include <stdlib.h>	/* for exit() definition */
#include <time.h>	/* for clock_gettime */

#include "../lib.h"

#define BUFFER_SIZE 65507 // MAX UDP packet size
#define ACK_SIZE 4
#define NUM_ITERATIONS 10
#define BILLION 1000000000L

int main(int argc, char *argv[])
{
	int timeout;
	if (argc < 3) {
		printf("Incorrect invocation. Correct usage: ./client <timeout> <file>\n");
		exit(1);
	}
	sscanf(argv[1], "%d", &timeout);
	char *filename = argv[2];
	FILE *fp;

	uint64_t total_time = 0;
	struct timespec start, end;

	char message[BUFFER_SIZE];
	struct sockaddr_in addr, addr2;
	char ack_message[ACK_SIZE];

	int sd = UDP_Open(20000);
	int rc = UDP_FillSockAddr(&addr, "machine.cs.wisc.edu", 10000);

	// Benchmark run
	for (int i = 1; i <= NUM_ITERATIONS; ++i) {
		// Open the file
		fp = fopen(filename, "rb");
		if (fp == NULL) {
			fprintf(stderr, "Can't open the input file!\n");
	  	exit(1);
		}
		uint64_t iter_time = 0;
		// Read and send the entire file, one buffer at a time
		while (fread(message, 1, sizeof(message), fp) > 0) {
				clock_gettime(CLOCK_REALTIME, &start);	/* mark start time */
				rc = UDP_Timeout_Retry(sd, &addr, message, &addr2, ack_message, timeout, BUFFER_SIZE);
				clock_gettime(CLOCK_REALTIME, &end);	/* mark end time */

				iter_time += BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		}

		printf("Iteration %d\tTotal time taken = %llu nanoseconds\n", i, (long long unsigned int) iter_time);
		total_time += iter_time;
		fclose(fp);
	}
	double mean_time = (double)(((double) total_time) / ((double) NUM_ITERATIONS));
	printf("Mean time taken = %f nanoseconds\n", mean_time);
	return 0;
}
