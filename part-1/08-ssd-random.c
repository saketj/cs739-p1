#include <stdio.h>
#include <stdint.h>	/* for uint64 definition */
#include <stdlib.h>	/* for exit() definition */
#include <time.h>	/* for clock_gettime */


#define BUFFER_SIZE 4096 // 4 KB
#define NUM_ITERATIONS 10
#define BILLION 1000000000L

double median(int n, uint64_t *x) {
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
	int i;
	if (argc < 2) {
		printf("Incorrect invocation. Correct usage: ./client <file>\n");
		exit(1);
	}
	char *filename = argv[1];
	FILE *fp;
	
	uint64_t total_time = 0;
	struct timespec start, end;

	char message[BUFFER_SIZE];
	uint64_t *results = (uint64_t *) malloc(NUM_ITERATIONS * sizeof(uint64_t));
	// Open the file
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Can't open the input file!\n");
	exit(1);
	}

	// Benchmark run
	for (i = 0; i < NUM_ITERATIONS; ++i) {
	uint64_t iter_time = 0;
		fseek(fp, (rand() % 1000) * BUFFER_SIZE, SEEK_CUR);
		// Read and send the entire file, one buffer at a time
		clock_gettime(CLOCK_REALTIME, &start);	/* mark start time */
	        fread(message, 1, sizeof(message), fp);
		clock_gettime(CLOCK_REALTIME, &end);	/* mark end time */

		iter_time += BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		results[i] = iter_time;
		printf("Iteration %d\tTotal time taken = %llu nanoseconds\n", i, (long long unsigned int) iter_time);
	}
	double median_time = median(NUM_ITERATIONS, results);
	printf("Median time taken = %f nanoseconds for %s data.\n", median_time, filename);
	free(results);
	
	fclose(fp);
	return 0;
}
