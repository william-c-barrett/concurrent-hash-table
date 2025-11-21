#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// Initialize logger (open hash.log file)
void logger_init(void);

// Close logger
void logger_close(void);

// Get current timestamp in microseconds
long long current_timestamp(void);

// Log thread waiting for turn
void log_waiting(int priority);

// Log thread awakened for work
void log_awakened(int priority);

// Log read lock acquired
void log_read_lock_acquired(int priority);

// Log read lock released
void log_read_lock_released(int priority);

// Log write lock acquired
void log_write_lock_acquired(int priority);

// Log write lock released
void log_write_lock_released(int priority);

// Log command execution
void log_command(int priority, const char *command);

#endif // LOGGER_H
