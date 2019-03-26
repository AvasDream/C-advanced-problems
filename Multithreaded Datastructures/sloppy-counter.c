#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include "threads.h"

#define THREADS 10
#define THRESHOLD 20
#define MAX 100

typedef struct counter {
  int			value;
  pthread_mutex_t	globallock;
  int 			local[THREADS];
  pthread_mutex_t	locallock[THREADS];
  int			threshold;
} counter;

typedef struct threadWrapper {
  counter * count;
  int id;
} threadWrapper;

void init(counter *c, int threshold)  {
  c->threshold = threshold;
  c->value = 0;
  Pthread_mutex_init(&c->globallock);
  for (int i=0; i < THREADS; i++) {
    c->local[i] = 0;
    Pthread_mutex_init(&c->locallock[i]);
  }
}

void increment(counter *c, int threadId) {
  Pthread_mutex_lock(&c->locallock[threadId]);
  c->local[threadId] += 1;
  if (c->local[threadId] >= c->threshold) {
    Pthread_mutex_lock(&c->globallock);
    c->value += c->local[threadId];
    Pthread_mutex_unlock(&c->globallock);
    c->local[threadId] = 0;
  }
  //printf("%d\n", c->local[threadId] );
  Pthread_mutex_unlock(&c->locallock[threadId]);
}

int get(counter *c) {
  Pthread_mutex_lock(&c->globallock);
  int rc = c->value;
  Pthread_mutex_unlock(&c->globallock);
  return rc;
}

// Other Solution write argument type for Pthread_create
void *incrementWrapper(threadWrapper *thread) {
  for (int i=0;i<MAX;i++) {
      increment(thread->count, thread->id);
  }
}
int main(int argc, char const *argv[]) {

  struct timeval start;
  struct timeval end;
  double timer = 0.0;

  counter count;
  init(&count, THRESHOLD);
  pthread_t threads[THREADS];

  printf("Threads: %d\n", THREADS);
  printf("Threshold: %d\n", THRESHOLD);
  gettimeofday(&start, NULL);
  // Create Threads
  for (size_t i = 0; i < THREADS; i++) {
    threadWrapper tw;
    tw.id = i;
    tw.count = &count;
    pthread_t p;
    threads[i] = p;
    Pthread_create(&threads[i], NULL, incrementWrapper, &tw);
    //Pthread_create(&threads[i], NULL, increment, &count);
  }
  // Join Threads
  for (size_t i = 0; i < THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  gettimeofday(&end, NULL);
  timer = (end.tv_sec - start.tv_sec) * 1000.0;
  timer += (end.tv_usec - start.tv_usec) / 1000.0;
  printf("Counter: %d\n", get(&count) );
  printf("Time: %f ms\n", timer);
  return 0;
}
