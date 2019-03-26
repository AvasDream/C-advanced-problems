#include <pthread.h>
#include <assert.h>

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

// Maybe a problem if mutex attributes are needed
void Pthread_mutex_init(pthread_mutex_t * mutex) {
  int rc = pthread_mutex_init(mutex, NULL);
  assert(rc == 0);
}
