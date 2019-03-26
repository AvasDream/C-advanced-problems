#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "threads.h"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

sem_t OneArrived, TwoArrived;

void *child_1(void *arg) {
    printf("child 1: before\n");
    sleep(1);
    sem_post(&OneArrived);
    sleep(1);
    sem_wait(&TwoArrived);
    printf("child 1: after\n");
    return NULL;
}

void *child_2(void *arg) {
    printf("child 2: before\n");
    sleep(1);
    sem_post(&TwoArrived);
    sem_wait(&OneArrived);
    printf("child 2: after\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2;
    printf("parent: begin\n");
    // init semaphores here
    sem_init(&OneArrived,0,0);
    sem_init(&TwoArrived,0,0);
    Pthread_create(&p1, NULL, child_1, NULL);
    Pthread_create(&p2, NULL, child_2, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("parent: end\n");
    return 0;
}
