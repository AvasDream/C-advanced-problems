//Modified
#include <stdio.h>
#include "mythreads.h"

int balance = 0;
pthread_mutex_t lock;

void* worker() {
    pthread_mutex_lock(&lock);
    balance++; // unprotected access
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    // Initialize Mutex
    int rc = pthread_mutex_init(&lock, NULL);
    assert(rc==0);
    Pthread_create(&p, NULL, worker, NULL);
    pthread_mutex_lock(&lock);
    balance++; // unprotected access
    pthread_mutex_unlock(&lock);
    Pthread_join(p, NULL);
    return 0;
}
