#include <stdio.h>	/* for printf */
#include <stdint.h>	/* for uint64 definition */
#include <stdlib.h>	/* for exit() definition */
#include <time.h>	/* for clock_gettime */
#include <unistd.h> /* for sleep */

#define BILLION 1000000000L

int main(int argc, char **argv)
{
	uint64_t diff;
	struct timespec start, end;
	int num_iterations = 1;

	/* measure system wide real time */
	clock_gettime(CLOCK_REALTIME, &start);	/* mark start time */
	//num_iterations = 2;
	clock_gettime(CLOCK_REALTIME, &end);	/* mark the end time */

	diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);

	exit(0);
}
