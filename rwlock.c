#include "rwlock.h"
#include "logger.h"
#include <stdio.h>

void rwlock_init(rwlock_t *lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->cond, NULL);
    lock->readers = 0;
    lock->writer = 0;
    lock->writers_waiting = 0;
    lock->thread_priority = 0;
}

void rwlock_set_priority(rwlock_t *lock, int priority) {
    lock->thread_priority = priority;
}

void rwlock_destroy(rwlock_t *lock) {
    pthread_mutex_destroy(&lock->mutex);
    pthread_cond_destroy(&lock->cond);
}

void rwlock_acquire_read(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->writer > 0 || lock->writers_waiting > 0) {
        pthread_cond_wait(&lock->cond, &lock->mutex);
    }
    lock->readers++;
    pthread_mutex_unlock(&lock->mutex);
    log_read_lock_acquired(lock->thread_priority);
}

void rwlock_release_read(rwlock_t *lock) {
    log_read_lock_released(lock->thread_priority);
    pthread_mutex_lock(&lock->mutex);
    lock->readers--;
    if (lock->readers == 0) {
        pthread_cond_broadcast(&lock->cond);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_acquire_write(rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->writers_waiting++;
    while (lock->writer > 0 || lock->readers > 0) {
        pthread_cond_wait(&lock->cond, &lock->mutex);
    }
    lock->writers_waiting--;
    lock->writer = 1;
    pthread_mutex_unlock(&lock->mutex);
    log_write_lock_acquired(lock->thread_priority);
}

void rwlock_release_write(rwlock_t *lock) {
    log_write_lock_released(lock->thread_priority);
    pthread_mutex_lock(&lock->mutex);
    lock->writer = 0;
    pthread_cond_broadcast(&lock->cond);
    pthread_mutex_unlock(&lock->mutex);
}
