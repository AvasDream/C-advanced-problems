#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
    int fd1[2];
    int fd2[2];
    int time[2];
    pipe(fd1);
    pipe(fd2);
    pipe(time);
    struct timeval start;
    struct timeval end;
    char payload[] = "Hello World"; 
    // Run only on one processor
    sched_setaffinity(0,1,NULL);
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
	// child
	gettimeofday(&start,NULL);
    // Write to 2
	write(fd2[0],payload, strlen(payload)+1);
    char buffer[30]; 
    read(fd2[1], &buffer, strlen(buffer) +1 );
    write(time[0], &start, sizeof(start) + 1 );
	close(fd1[1]);
    } else {
    // parent
    close(fd1[0]);
    close(fd2[1]);
    write(fd1[1], payload, strlen(payload)+1);
    char buffer[30]; 
    read(fd2[0], &buffer, strlen(buffer) +1 );
    gettimeofday(&start,NULL);
    double elapsed = 0.0;
    read(time[1], &end, sizeof(end)+1);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0; 
    printf("Time elapsed: %f\n", elapsed);
    }
    return 0;
}
