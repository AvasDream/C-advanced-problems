#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>

void main() {

struct timeval start;
struct timeval end;
system("touch nullbyte");
gettimeofday(&start,NULL);

system("cat nullbyte");

gettimeofday(&end,NULL);

double elapsed = 0.0;
elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
printf("Time elapsed: %f\n", elapsed);

system("rm nullbyte");
}
