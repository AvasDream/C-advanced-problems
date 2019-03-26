#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include "threads.h"

#define THREADS 10
#define MAX 1000
#define DEQUEUE 100

typedef struct node {
  int value;
  struct node *next;
} node;

typedef struct queue {
  node *head;
  node *tail;
  pthread_mutex_t headlock;
  pthread_mutex_t taillock;
} queue;

node* initNode(int value, node *next) {
  node *newNode = (node*) malloc(sizeof(node));
  newNode->value = value;
  newNode->next = next;
  return newNode;
}

queue* initQueue() {
  queue *q = (queue*) malloc(sizeof(queue));
  node *tmp = initNode(0,NULL);
  q->head = q->tail = tmp;
  //q->tail = tmp;
  Pthread_mutex_init(&q->headlock);
  Pthread_mutex_init(&q->taillock);
  //printf("Hello end of initQueue\n");
  return q;
}

void enqueue(queue *q, int value) {
  node *tmp = initNode(value, NULL);
  Pthread_mutex_lock(&q->taillock);
  q->tail->next = tmp;
  q->tail = tmp;
  Pthread_mutex_unlock(&q->taillock);
  return;
}

// No need to return current head value, because we are
// just measuring the time
void dequeue(queue *q) {
  Pthread_mutex_lock(&q->headlock);
  node *current = q->head;
  node *newHead = current->next;
  if (newHead == NULL) {
    Pthread_mutex_unlock(&q->headlock);
    return;
  }
  q->head = newHead;
  Pthread_mutex_unlock(&q->headlock);
  free(current);
  return;
}

void enqueueWrapper(queue *q) {
  for (int i = 0; i < MAX; i++) {
    enqueue(q, i);
    //printf("AddToListWrapper\n");
  }
  for (int i = 0; i < DEQUEUE; i++) {
    dequeue(q);
  }
}




int main(int argc, char const *argv[]) {
  struct timeval start;
  struct timeval end;
  double timer = 0.0;

  queue *q = initQueue();
  //initQueue(&q);

  pthread_t threads[THREADS];
  printf("Threads: %d\n", THREADS);
  gettimeofday(&start, NULL);

  for (size_t i = 0; i < THREADS; i++) {
    pthread_t p;
    threads[i] = p;
    Pthread_create(&threads[i], NULL, enqueueWrapper, q);
  }
  for (size_t i = 0; i < THREADS; i++) {
  pthread_join(threads[i], NULL);
  }
  gettimeofday(&end, NULL);
  timer = (end.tv_sec - start.tv_sec) * 1000.0;
  timer += (end.tv_usec - start.tv_usec) / 1000.0;
  printf("Time: %f ms\n", timer);
  return 0;
}
