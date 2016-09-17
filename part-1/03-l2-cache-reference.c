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
#define L2_ARR_SIZE 50000 // 50000 elements => 200K bytes => less than size of L2 cache
#define L2_STEP_SIZE 10000 // 10K elements => 40K bytes => more than the size of L1 cache
#define MEASURE_PERF_COUNTERS

#ifdef MEASURE_PERF_COUNTERS
static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
    return ret;
}
#endif

int main(int argc, char **argv)
{
    uint64_t base, total_time;
    struct timespec start, end;

    int i,j, steps = L2_ARR_SIZE / L2_STEP_SIZE + 1;
    int num = 0;
    int idx = 0;
    
    int *arr = (int *)malloc(sizeof(int) * L2_ARR_SIZE);
    for (i = 0; i < L2_ARR_SIZE; ++i) {
       arr[i] = i;
    }
    
    // Find the baseline measurement without memory access.    
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */      
    for (i = 0; i < NUM_ITERATIONS; ++i) {
      for (j = 0; j < steps; ++j) {
	idx = (j * L2_STEP_SIZE + i) % L2_ARR_SIZE;
      }
    } 
    clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */
    base = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    
    // Warm the cache.
    for (i = 0; i < NUM_ITERATIONS; ++i) {
       idx = i % L2_ARR_SIZE;
       num = arr[idx];
    } 
    
    // Now do measurement with memory access.
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */      
    for (i = 0; i < NUM_ITERATIONS; ++i) {
      for (j = 0; j < steps; ++j) {
	idx = (j * L2_STEP_SIZE + i) % L2_ARR_SIZE;
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


void test_L1_cache_counters() {
    uint64_t diff;
    struct timespec start, end;

    #ifdef MEASURE_PERF_COUNTERS
    struct perf_event_attr pe;
    long long count;
    int fd;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HW_CACHE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = (PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
       fprintf(stderr, "Error opening leader %llx\n", pe.config);
       exit(EXIT_FAILURE);
    }
    #endif

    int i;
    int num = 0; 
    // Warm the cache with an array
    int *arr = (int *)malloc(sizeof(int) * L2_ARR_SIZE);
    for (i = 0; i < L2_ARR_SIZE; ++i) {
       arr[i] = i;
    }
    for (i = 0; i < NUM_ITERATIONS; ++i) {
       num = arr[0];
    }
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);	/* mark start time */ 
    
    #ifdef MEASURE_PERF_COUNTERS
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    #endif
   
    for (i = 0; i < NUM_ITERATIONS; ++i) { 
       num = arr[0];
    } 
    
    #ifdef MEASURE_PERF_COUNTERS
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    #endif

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);	/* mark the end time */
    
    printf("Value of num = %d\n", num);
    
    #ifdef MEASURE_PERF_COUNTERS
    read(fd, &count, sizeof(long long));	
    printf("%lld L1 cache misses\n", count); 
    close(fd);
    #endif

    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    double cache_ref_time = (double)(diff) / (double)(NUM_ITERATIONS);
    printf("L1 cache reference time = %f nanoseconds \n", cache_ref_time);
    
    free(arr);
}
