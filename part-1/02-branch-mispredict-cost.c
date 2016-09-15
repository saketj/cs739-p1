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
#define NUM_ITERATIONS 10 // 1 million

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
    return ret;
}

int
main(int argc, char **argv)
{
    struct perf_event_attr pe;
    long long count_1, count_2;
    int fd;

    uint64_t diff_1, diff_2;
    struct timespec start, end;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_BRANCH_MISSES;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
       fprintf(stderr, "Error opening leader %llx\n", pe.config);
       exit(EXIT_FAILURE);
    }

    int i; 
    
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    
    int sum = 0; 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);	/* mark start time */
    for (i = 0; i <= NUM_ITERATIONS; ++i) { // some branch mispredictions in the for-loop condition
	if ((i & 1) == 0) {
		sum += 1;
	}
	else {
		sum -= 1;
	}
    } 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);	/* mark the end time */
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    
    read(fd, &count_1, sizeof(long long));
	
    diff_1 = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    printf("%lld branch mispredicts in %llu nanoseconds \n", count_1, (long long unsigned int) diff_1);
    printf("sum = %d after %d iterations\n", sum, NUM_ITERATIONS);
    
    printf("Re-running the experiment again when there will be different mispredictions...\n");
    
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    
    sum = 0;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);	/* mark start time */
    for (i = 0; i <= NUM_ITERATIONS; ++i) { // some branch mispredictions in the for-loop condition
	if ((i & 4) == 0) {
		sum += 1;
	}
	else {
		sum += 2;
	}
    } 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);	/* mark the end time */
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    
    read(fd, &count_2, sizeof(long long));
	
    diff_2 = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    printf("%lld branch mispredicts in %llu nanoseconds \n", count_2, (long long unsigned int) diff_2);
    printf("sum = %d after %d iterations\n", sum, NUM_ITERATIONS);
    
    double branch_mispredict_cost = 0.0;
    if (count_1 < count_2) {
	branch_mispredict_cost = ((double)(diff_2 - diff_1)) / ((double)(count_2 - count_1));
    } else {
    	branch_mispredict_cost = ((double)(diff_1 - diff_2)) / ((double)(count_1 - count_2));
    }
    printf("\nBranch misprediction cost = %f nanoseconds\n", branch_mispredict_cost);
    close(fd);
}

