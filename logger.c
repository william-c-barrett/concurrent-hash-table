#include "logger.h"
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void logger_init(void) {
    log_file = fopen("hash.log", "w");
    if (log_file == NULL) {
        perror("Failed to open hash.log");
    }
}

void logger_close(void) {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
}

long long current_timestamp(void) {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long microseconds = ((long long)te.tv_sec * 1000000LL) + te.tv_usec;
    return microseconds;
}

static void log_with_timestamp(int priority, const char *message) {
    if (log_file != NULL) {
        long long ts = current_timestamp();
        pthread_mutex_lock(&log_mutex);
        fprintf(log_file, "%lld: THREAD %d %s\n", ts, priority, message);
        fflush(log_file);
        pthread_mutex_unlock(&log_mutex);
    }
}

void log_waiting(int priority) {
    log_with_timestamp(priority, "WAITING FOR MY TURN");
}

void log_awakened(int priority) {
    log_with_timestamp(priority, "AWAKENED FOR WORK");
}

void log_read_lock_acquired(int priority) {
    log_with_timestamp(priority, "READ LOCK ACQUIRED");
}

void log_read_lock_released(int priority) {
    log_with_timestamp(priority, "READ LOCK RELEASED");
}

void log_write_lock_acquired(int priority) {
    log_with_timestamp(priority, "WRITE LOCK ACQUIRED");
}

void log_write_lock_released(int priority) {
    log_with_timestamp(priority, "WRITE LOCK RELEASED");
}

void log_command(int priority, const char *command) {
    if (log_file != NULL) {
        long long ts = current_timestamp();
        pthread_mutex_lock(&log_mutex);
        fprintf(log_file, "%lld: THREAD %d %s\n", ts, priority, command);
        fflush(log_file);
        pthread_mutex_unlock(&log_mutex);
    }
}
