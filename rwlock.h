#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int readers;
    int writer;
    int writers_waiting;
    int thread_priority; // For logging purposes
} rwlock_t;

void rwlock_init(rwlock_t *lock);
void rwlock_destroy(rwlock_t *lock);
void rwlock_set_priority(rwlock_t *lock, int priority);
void rwlock_acquire_read(rwlock_t *lock);
void rwlock_release_read(rwlock_t *lock);
void rwlock_acquire_write(rwlock_t *lock);
void rwlock_release_write(rwlock_t *lock);

#endif // RWLOCK_H
