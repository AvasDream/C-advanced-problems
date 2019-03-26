#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include "threads.h"

#define THREADS 10
#define MAX 100

typedef struct node {
  int value;
  struct node *next;
  pthread_mutex_t *lock;
} node;

typedef struct nodeList {
  node *head;
} nodeList;

node* initNode(int value, node *next) {
  node *newNode = (node *) malloc(sizeof(node));
  newNode->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  Pthread_mutex_init(newNode->lock);
  newNode->value = value;
  newNode->next = next;
  return newNode;
}

nodeList* initList() {
  nodeList *listpointer = (nodeList *) malloc(sizeof(nodeList));
  listpointer->head = initNode(0,NULL);
  return listpointer;
}


void addToList(nodeList *listpointer,int value) {
  // Lock elem + elem->next
  //printf("Hello from addToList\n");
  node *current = listpointer->head;
  Pthread_mutex_lock(current->lock);

  // Case empty list
  if (current->next == NULL) {
    node *tmp = initNode(value,NULL);
    current->next = tmp;
    Pthread_mutex_unlock(current->lock);
    //printf("Add first element\n");
    return;
  }

  //Just to init previous, but maybe critical
  node *previous = current;
  while(current->next != NULL) {
    previous = current;
    current = current->next;
    //Release Current Lock Next
    //printf("Iterate through List\n");
    Pthread_mutex_lock(current->lock);
    Pthread_mutex_unlock(previous->lock);
  }

  node *newNode = initNode(value,current->next);
  current->next = newNode;
  //printf("Added element\n");
  Pthread_mutex_unlock(current->lock);
  return;
}

void addToListWrapper(nodeList *list) {
  for (int i = 0; i < MAX; i++) {
    addToList(list, i);
    //printf("AddToListWrapper\n");
  }
}
int main(int argc, char const *argv[]) {

  struct timeval start;
  struct timeval end;
  double timer = 0.0;

  nodeList *list = initList();


  pthread_t threads[THREADS];

  printf("Threads: %d\n", THREADS);
  gettimeofday(&start, NULL);
  for (size_t i = 0; i < THREADS; i++) {
    pthread_t p;
    threads[i] = p;
    Pthread_create(&threads[i], NULL, addToListWrapper, list);
  }
  for (size_t i = 0; i < THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  gettimeofday(&end, NULL);
  timer = (end.tv_sec - start.tv_sec) * 1000.0;
  timer += (end.tv_usec - start.tv_usec) / 1000.0;
  //printf("Counter: %d\n", get(&count) );
  printf("Time: %f ms\n", timer);
  return 0;
}
