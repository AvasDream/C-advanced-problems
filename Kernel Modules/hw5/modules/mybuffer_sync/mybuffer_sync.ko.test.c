#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <pthread.h>
#include <assert.h>

void Pthread_create(pthread_t *thread, const pthread_attr_t *attributes,
                    void *(*function) (void *), void *arguments);
void Pthread_mutex_lock(pthread_mutex_t * mutex);
void Pthread_mutex_unlock(pthread_mutex_t * mutex);
void Pthread_mutex_init(pthread_mutex_t * mutex);

void Pthread_create(pthread_t *thread, const pthread_attr_t *attributes,
                    void *(*function) (void *), void *arguments) {
  int rc = pthread_create(thread, attributes, function, arguments);
  assert(rc == 0);
}

void Pthread_mutex_lock(pthread_mutex_t * mutex) {
  int rc = pthread_mutex_lock(mutex);
  assert(rc == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t * mutex) {
  int rc = pthread_mutex_unlock(mutex);
  assert(rc == 0);
}


void Pthread_mutex_init(pthread_mutex_t * mutex) {
  int rc = pthread_mutex_init(mutex, NULL);
  assert(rc == 0);
}

void *test() {
	char buffer[64];
	int file = open("test", "w");
	int rc = write(file, "hello", 6);
	if (rc < 0) {
		exit(-1);
	}
	close(file);
	int readfile = open("test", "r");
	rc = read(readfile, buffer, 6);
	if (rc < 0) {
		exit(-1);
	}
	printf("Hello from: %d\nRead: %s", (int) pthread_self(), buffer);
}

int main(int argc, char const *argv[])
{
	int loop = 10;
	pthread_t threads[loop];
	for (size_t i = 0; i < loop; i++) {
  		Pthread_create(&threads[i], NULL, test, NULL);
	}
	for (size_t i = 0; i < sizeof(threads)/sizeof(threads[0]); i++) {
  		pthread_join(threads[i], NULL);
	}
	printf("Test passed\n");
    return 0;
}
