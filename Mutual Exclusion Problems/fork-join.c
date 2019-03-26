#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "threads.h"

sem_t s;

void *child(void *arg) {
    sleep(5);
    printf("child\n");
    sem_post(&s);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    // Initial value is 0 because we want to use it as signal
    // not as lock
    sem_init(&s,0,0);
    printf("parent: begin\n");
    Pthread_create(&p, NULL, child, NULL);
    sem_wait(&s);
    printf("parent: end\n");
    return 0;
}
