#ifndef _THREADS_H_
#define _THREADS_H_

void Pthread_create(pthread_t *thread, const pthread_attr_t *attributes,
                    void *(*function) (void *), void *arguments);
void Pthread_mutex_lock(pthread_mutex_t * mutex);
void Pthread_mutex_unlock(pthread_mutex_t * mutex);
void Pthread_mutex_init(pthread_mutex_t * mutex);

#endif
