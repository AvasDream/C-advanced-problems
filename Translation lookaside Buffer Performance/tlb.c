#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
 #include <pthread.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc <= 2) {
    printf("Missing Arguments!\n");
    printf("tlb <Number of pages> <number of trials>\n");
    exit(1);
    }
    int pagesize, numpages, numtrials;
    double elapsed;
    struct timeval start;
    struct timeval end;
    pagesize = 4096;
    numpages = atoi(argv[1]);
    numtrials = atoi (argv[2]);
    int array[numpages * pagesize / sizeof(int)];
    printf("Page size: %d\n", pagesize);
    printf("Num Pages: %d\n", numpages);
    printf("Num trials: %d\n", numtrials);
    // CPU thread pinning
    cpu_set_t cpuset;
    pthread_t thread;
    thread = pthread_self();
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    // One integer per page is updated in array
    double avg = 0;
    int access = 0;
    int jump = pagesize / sizeof(int);
    for (int i = 0; i < numtrials; i++) {
      printf("Trial: %d\n", i);
      for (int i = 0; i < numpages * jump; i += jump) {
        gettimeofday(&start,NULL);
        array[i] += 1;
        gettimeofday(&end,NULL);
        elapsed = (end.tv_sec - start.tv_sec) * 1000.0;
        elapsed += (end.tv_usec - start.tv_usec) / 1000.0;
        avg += elapsed;
        // Time in ms, format: 00.00000
        printf("access: %d  - %2.15f ms\n",access, elapsed);
        access += 1;
      }
      printf("Trial: %d Average: %2.10f\n", i, avg / (numpages * jump) );
      avg = 0;
    }
    return 0;
}


