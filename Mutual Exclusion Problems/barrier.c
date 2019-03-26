#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "threads.h"



// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.

typedef struct __barrier_t {
    //Number of threads
    int threads;
    // Number of threads arrived
    int count;
    // Mutex for count
    sem_t mutex;
    // Barrier which is locked until all threads arrive
    sem_t barrier;
} barrier_t;


// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    b->threads = num_threads;
    b->count = 0;
    // X=1 bc lock
    sem_init(&b->mutex,0,1);
    // X=0 bc for signaling
    sem_init(&b->barrier,0,0);
}

void barrier(barrier_t *b) {
    //increment number of threads
    sem_wait(&b->mutex);
    b->count++;
    sem_post(&b->mutex);
    if (b->count == b->threads) {
      sem_post(&b->barrier);
    }
    sem_wait(&b->barrier);
    // add this so no deadlock will occur
    sem_post(&b->barrier);
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}


// run with a single argument indicating the number of
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);

    int i;
    for (i = 0; i < num_threads; i++) {
    	t[i].thread_id = i;
    	Pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (i = 0; i < num_threads; i++)
        pthread_join(p[i], NULL);
        printf("parent: end\n");
    return 0;
}
