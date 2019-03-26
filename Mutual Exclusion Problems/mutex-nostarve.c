#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include "threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//
/*

Phase 1: drehkreuz1 ist offen, drehkreuz2 ist geschlossen
Threads sammeln sich in warteraum2
Phase 2: drehkreuz1 ist geschlossen, drehkreuz2 ist offen
Threads können kritischen Bereich betreten.


*/

typedef struct __ns_mutex_t {
  //Threads in waiting room
  int warteraum1;
  int warteraum2;
  // Protection for counters
  sem_t mutex;
  // Turnsiles = Drehkreuze
  sem_t drehkreuz1;
  sem_t drehkreuz2;
} ns_mutex_t;

int critical = 0;

void ns_mutex_init(ns_mutex_t *m) {
  sem_init(&m->mutex,0,1);
  sem_init(&m->drehkreuz1,0,1);
  sem_init(&m->drehkreuz2,0,0);
  m->warteraum1 = 0;
  m->warteraum2 = 0;
}

void ns_mutex_acquire(ns_mutex_t *m) {
  sem_wait(&m->mutex);
  m->warteraum1 += 1;   //warteraum1 start
  sem_post(&m->mutex);

  sem_wait(&m->drehkreuz1);
  m->warteraum2 += 1;
  sem_wait(&m->mutex);
  m->warteraum1 -= 1;   // warteraum1 end
                        // warteraum2 start
  if (m->warteraum1 == 0) {
    sem_post(&m->mutex);
    sem_post(&m->drehkreuz2);
  } else {
    sem_post(&m->mutex);
    sem_post(&m->drehkreuz1);
  }
  sem_wait(&m->drehkreuz2);
  m->warteraum2 -= 1;   //warteraum2 end
}

void ns_mutex_release(ns_mutex_t *m) {
  // If warteraum 1 is empty open warteraum1
  // else open warteraum2
  if (m->warteraum2 == 0) {
    sem_post(&m->drehkreuz1);
  } else {
    sem_post(&m->drehkreuz2);
  }
}

typedef struct __tinfo_t {
    int thread_id;
    ns_mutex_t *mutex;
} tinfo_t;

void *worker(void *arg) {
  tinfo_t *t = (tinfo_t *) arg;
  printf("worker %d: before\n", t->thread_id);
  ns_mutex_acquire(t->mutex);
  printf("worker %d: critical\n", t->thread_id);
  critical++;
  ns_mutex_release(t->mutex);
  printf("worker %d: after\n", t->thread_id);
  return NULL;
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");

    int num_threads = 10;
    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    ns_mutex_t mutex;
    ns_mutex_init(&mutex);

    int i;
    for (i = 0; i < num_threads; i++) {
    	t[i].thread_id = i;
      t[i].mutex = &mutex;
    	Pthread_create(&p[i], NULL, worker, &t[i]);
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(p[i], NULL);
    }
    printf("Counter: %d\n", critical);    
    printf("parent: end\n");
    return 0;
}
