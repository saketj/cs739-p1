#include <stdio.h>	/* for printf */
#include <stdint.h>	/* for uint64 definition */
#include <stdlib.h>	/* for exit() definition */
#include <time.h>	/* for clock_gettime */
#include <unistd.h> /* for sleep */

#define BILLION 1000000000L
#define loop 100000
float mean(int m, int a[]) {
    int sum=0, i;
    for(i=0; i<m; i++)
        sum+=a[i];
    return((float)sum/m);
}


float median(int n, int x[]) {
    float temp;
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

int main(int argc, char **argv)
{
	int arr[loop];
	int i=0;  
	uint64_t diff;
	struct timespec start, end;
	int num_iterations = 1;

	/* measure system wide real time */
	for (i=0; i< loop; i++)
	{

		//clock_gettime(CLOCK_REALTIME, &start);	/* mark start time */
		clock_gettime(CLOCK_MONOTONIC, &start);  
		num_iterations = 2 + 3;
		clock_gettime(CLOCK_MONOTONIC, &end);
		//clock_gettime(CLOCK_REALTIME, &end);	/* mark the end time */
		//num_iterations = 2;

		diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
		arr[i] = (long long unsigned int) diff; 
		//os.system("sudo sh -c 'sync; echo 3 > /proc/sys/vm/drop_caches'"); 
	}
	
	int j = 0, min =1000 ; 
	for ( i = 0 ; i < loop ; i++ ) 
    	{
        	if ( arr[i] < min ) 
        	{
           		min = arr[i];
        	}
    	} 
	int max = 0;  
	for ( i = 0 ; i < loop ; i++ )
        {
                if ( arr[i] > max )
                {
                        max = arr[i];
                }
        }

	printf("first = %d,  min = %d,max = %d,  mean = %f, median = %f nanoseconds\n", arr[0], min,max, mean(loop, arr), median(loop,arr));

	exit(0);
}
