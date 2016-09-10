#include <stdio.h>
#include "rdtsc.h"

int main(int argc, char* argv[])
{
  int i = 0, j;
  unsigned long long a,b, min_time = 1000L, max_time = 0, last_time;
  for (i = 0; i < 1000000; ++i) {
  	a = rdtsc();
  	b = rdtsc();
	last_time = b - a;
	if (last_time < min_time) {
		min_time = last_time;
	}
	if (last_time > max_time) {
		max_time = last_time;
	}
  }
  printf("min_time = %llu max_time = %llu\n", min_time, max_time);
  return 0;
}
