#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#define BILLION 1000000000L
#define NUM_ITERATIONS 1000000 // 1 million
#define ARR_SIZE 100000000 // 100 million elements => 400MB => less than size of L2 cache
#define STEP_SIZE 10000 // 10K elements => 40K bytes => more than the size of L1 cache

int main(int argc, char **argv)
{
    uint64_t base, total_time;
    struct timespec start, end;

    int i,j, steps = ARR_SIZE / STEP_SIZE + 1;
    int num = 0;
    int idx = 0;
    
    int *arr = (int *)malloc(sizeof(int) * ARR_SIZE);
    for (i = 0; i < ARR_SIZE; ++i) {
       arr[i] = i;
    }
    
    // Find the baseline measurement without memory access.    
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */      
    for (i = 0; i < NUM_ITERATIONS; ++i) {
      for (j = 0; j < steps; ++j) {
	idx = (j * STEP_SIZE + i) % ARR_SIZE;
      }
    } 
    clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */
    base = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    
    // Warm the cache.
    for (i = 0; i < NUM_ITERATIONS; ++i) {
       idx = i % ARR_SIZE;
       num = arr[idx];
    } 
    
    // Now do measurement with memory access.
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */      
    for (i = 0; i < NUM_ITERATIONS; ++i) {
      for (j = 0; j < steps; ++j) {
	idx = (j * STEP_SIZE + i) % ARR_SIZE;
	num = arr[idx];
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */
    total_time = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    
    printf("Value of num = %d\n", num);
    double cache_ref_time = (double)(total_time - base) / (double)(NUM_ITERATIONS);
    printf("L2 cache reference time = %f nanoseconds \n", cache_ref_time);
    
    free(arr);
    return 0;
}
