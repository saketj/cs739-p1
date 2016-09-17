#include <stdio.h>
#include <stdint.h>	/* for uint64 definition */
#include <stdlib.h>	/* for exit() definition */
#include <time.h>	/* for clock_gettime */

#include "../lib.h"

#define SERVER_IP "127.0.0.1"

#define BUFFER_SIZE 65507 // MAX UDP packet size
#define ACK_SIZE 4
#define NUM_ITERATIONS 1000
#define BILLION 1000000000L

double median(int n, int x[]) {
  double temp;
  int i, j;
  // the following two loops sort the array x in ascending order
  for(i=0; i<n-1; i++) {
    for(j=i+1; j<n; j++) {
      if(x[j] < x[i]) {
	// swap elements
	temp = x[i];
	x[i] = x[j];
	x[j] = temp;
      }
    }
  }

  if(n%2==0) {
    // if there is an even number of elements, return mean of the two elements in the middle
    return((x[n/2] + x[n/2 - 1]) / 2.0);
  } else {
    // else return the element in the middle
    return x[n/2];
  }
}

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
	int rc = UDP_FillSockAddr(&addr, SERVER_IP, 10000);
	int i;
	int results[NUM_ITERATIONS];

	// Benchmark run
	for (i = 0; i < NUM_ITERATIONS; ++i) {
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
		results[i] = iter_time;
		// printf("Iteration %d\tTotal time taken = %llu nanoseconds\n", i, (long long unsigned int) iter_time);
		fclose(fp);
	}
	double median_time = median(NUM_ITERATIONS, results);
	printf("Median time taken = %f nanoseconds for %s data.\n", median_time, filename);
	return 0;
}
