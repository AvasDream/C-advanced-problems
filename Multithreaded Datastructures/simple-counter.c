#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include "threads.h"

typedef struct counter {
  int value;
  pthread_mutex_t lock;
} counter;


// Counter

void init(counter *c) {
  c->value = 0;
  Pthread_mutex_init(&c->lock);
}

void increment(counter *c) {
  Pthread_mutex_lock(&c->lock);
  c->value++;
  Pthread_mutex_unlock(&c->lock);
}

void decrement(counter *c) {
  Pthread_mutex_lock(&c->lock);
  c->value--;
  Pthread_mutex_unlock(&c->lock);
}

int get(counter *c) {
  Pthread_mutex_lock(&c->lock);
  int rc = c->value;
  Pthread_mutex_unlock(&c->lock);
  return rc;
}

int main(int argc, char const *argv[]) {

  struct timeval start;
  struct timeval end;

  counter count;
  init(&count);

  int loop = 16;
  double timer = 0.0;

  pthread_t threads[loop];

  printf("Loops: %d\n", loop);

  gettimeofday(&start, NULL);
  for (size_t i = 0; i < loop; i++) {
    Pthread_create(&threads[i], NULL, increment, &count);
  }
  for (size_t i = 0; i < sizeof(threads)/sizeof(threads[0]); i++) {
    pthread_join(threads[i], NULL);
  }
  gettimeofday(&end, NULL);
  timer = (end.tv_sec - start.tv_sec) * 1000.0;
  timer += (end.tv_usec - start.tv_usec) / 1000.0;
  printf("Counter: %d\n", get(&count) );
  printf("Time: %f ms\n", timer);
  return 0;
}
