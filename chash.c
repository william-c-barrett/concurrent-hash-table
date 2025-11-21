#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "hash_table.h"
#include "hash.h"
#include "logger.h"

#define MAX_LINE_LENGTH 256
#define MAX_COMMANDS 1000

typedef enum {
    CMD_INSERT,
    CMD_DELETE,
    CMD_UPDATE,
    CMD_SEARCH,
    CMD_PRINT
} command_type_t;

typedef struct {
    command_type_t type;
    char name[50];
    uint32_t salary;
    uint32_t new_salary; // For update
    int priority;
} command_t;

static hashTable g_table;
static command_t commands[MAX_COMMANDS];
static int num_commands = 0;
static pthread_mutex_t priority_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t priority_cond = PTHREAD_COND_INITIALIZER;
static int current_priority = 1;
static int threads_finished = 0;
static int total_threads = 0;

// Thread argument structure
typedef struct {
    int thread_id;
    int priority;
    command_t *cmd;
} thread_arg_t;

// Parse a line from commands.txt
int parse_command(const char *line, command_t *cmd) {
    char line_copy[MAX_LINE_LENGTH];
    strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
    line_copy[MAX_LINE_LENGTH - 1] = '\0';
    
    // Remove trailing newline
    char *newline = strchr(line_copy, '\n');
    if (newline) *newline = '\0';
    if (line_copy[0] == '\0') return 0;
    
    char *token;
    char *saveptr;
    
    // Get command type
    token = strtok_r(line_copy, ",", &saveptr);
    if (token == NULL) return 0;
    
    if (strcmp(token, "threads") == 0) {
        // Skip threads line - it's just metadata
        return 0;
    } else if (strcmp(token, "insert") == 0) {
        cmd->type = CMD_INSERT;
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        strncpy(cmd->name, token, 49);
        cmd->name[49] = '\0';
        
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        cmd->salary = (uint32_t)atoi(token);
        
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        cmd->priority = atoi(token);
    } else if (strcmp(token, "delete") == 0) {
        cmd->type = CMD_DELETE;
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        strncpy(cmd->name, token, 49);
        cmd->name[49] = '\0';
        
        // Skip the middle parameter (0)
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        cmd->priority = atoi(token);
    } else if (strcmp(token, "update") == 0) {
        cmd->type = CMD_UPDATE;
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        strncpy(cmd->name, token, 49);
        cmd->name[49] = '\0';
        
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        cmd->new_salary = (uint32_t)atoi(token);
        
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        cmd->priority = atoi(token);
    } else if (strcmp(token, "search") == 0) {
        cmd->type = CMD_SEARCH;
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        strncpy(cmd->name, token, 49);
        cmd->name[49] = '\0';
        
        // Skip the middle parameter (0)
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        cmd->priority = atoi(token);
    } else if (strcmp(token, "print") == 0) {
        cmd->type = CMD_PRINT;
        // Skip the first two parameters (0,0)
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        
        token = strtok_r(NULL, ",", &saveptr);
        if (token == NULL) return 0;
        cmd->priority = atoi(token);
    } else {
        return 0; // Unknown command
    }
    
    return 1;
}

// Wait for turn based on priority
void wait_for_turn(int priority) {
    pthread_mutex_lock(&priority_mutex);
    while (priority != current_priority) {
        log_waiting(priority);
        pthread_cond_wait(&priority_cond, &priority_mutex);
    }
    log_awakened(priority);
    pthread_mutex_unlock(&priority_mutex);
}

// Signal next priority
void signal_next_priority(void) {
    pthread_mutex_lock(&priority_mutex);
    current_priority++;
    pthread_cond_broadcast(&priority_cond);
    pthread_mutex_unlock(&priority_mutex);
}

// Execute a command
void execute_command(command_t *cmd) {
    char log_msg[256];
    uint32_t hash;
    
    hash_table_set_priority(&g_table, cmd->priority);
    
    switch (cmd->type) {
        case CMD_INSERT:
            hash = jenkins_one_at_a_time_hash((const uint8_t*)cmd->name, strlen(cmd->name));
            snprintf(log_msg, sizeof(log_msg), "INSERT,%u,%s,%u", hash, cmd->name, cmd->salary);
            log_command(cmd->priority, log_msg);
            
            if (hash_table_insert(&g_table, cmd->name, cmd->salary) == 0) {
                printf("Inserted %u,%s,%u\n", hash, cmd->name, cmd->salary);
            } else {
                printf("Insert failed.  Entry %u is a duplicate.\n", hash);
            }
            break;
            
        case CMD_DELETE:
            hash = jenkins_one_at_a_time_hash((const uint8_t*)cmd->name, strlen(cmd->name));
            snprintf(log_msg, sizeof(log_msg), "DELETE,%u,%s", hash, cmd->name);
            log_command(cmd->priority, log_msg);
            
            // Search first with read lock to get salary value
            hashRecord *record = hash_table_search(&g_table, cmd->name);
            if (record != NULL) {
                uint32_t old_salary = record->salary;
                uint32_t old_hash = record->hash;
                char old_name[50];
                strncpy(old_name, record->name, 49);
                old_name[49] = '\0';
                free(record);
                
                // Now delete (will acquire write lock internally)
                if (hash_table_delete(&g_table, cmd->name) == 0) {
                    printf("Deleted record for %u,%s,%u\n", old_hash, old_name, old_salary);
                } else {
                    printf("Entry %u not deleted.  Not in database.\n", hash);
                }
            } else {
                printf("Entry %u not deleted.  Not in database.\n", hash);
            }
            break;
            
        case CMD_UPDATE:
            hash = jenkins_one_at_a_time_hash((const uint8_t*)cmd->name, strlen(cmd->name));
            uint32_t old_salary;
            snprintf(log_msg, sizeof(log_msg), "UPDATE,%u,%s,%u", hash, cmd->name, cmd->new_salary);
            log_command(cmd->priority, log_msg);
            
            if (hash_table_update(&g_table, cmd->name, cmd->new_salary, &old_salary) == 0) {
                printf("Updated record %u from %u,%s,%u to %u,%s,%u\n", 
                       hash, hash, cmd->name, old_salary, hash, cmd->name, cmd->new_salary);
            } else {
                printf("Updated failed.  Entry %u not found.\n", hash);
            }
            break;
            
        case CMD_SEARCH:
            hash = jenkins_one_at_a_time_hash((const uint8_t*)cmd->name, strlen(cmd->name));
            snprintf(log_msg, sizeof(log_msg), "SEARCH,%u,%s", hash, cmd->name);
            log_command(cmd->priority, log_msg);
            
            hashRecord *result = hash_table_search(&g_table, cmd->name);
            if (result != NULL) {
                printf("Found: %u,%s,%u\n", result->hash, result->name, result->salary);
                free(result);
            } else {
                printf("%s not found.\n", cmd->name);
            }
            break;
            
        case CMD_PRINT:
            snprintf(log_msg, sizeof(log_msg), "PRINT");
            log_command(cmd->priority, log_msg);
            
            int count;
            hashRecord **records = hash_table_get_all_sorted(&g_table, &count);
            printf("Current Database:\n");
            if (count > 0) {
                int i;
                for (i = 0; i < count; i++) {
                    printf("%u,%s,%u\n", records[i]->hash, records[i]->name, records[i]->salary);
                    free(records[i]); // Free the copied record
                }
                free(records); // Free the array
            }
            break;
    }
}

// Thread function
void* thread_func(void *arg) {
    thread_arg_t *targ = (thread_arg_t*)arg;
    
    // Wait for turn based on priority
    wait_for_turn(targ->priority);
    
    // Execute command
    execute_command(targ->cmd);
    
    // Signal next priority
    signal_next_priority();
    
    pthread_mutex_lock(&priority_mutex);
    threads_finished++;
    pthread_mutex_unlock(&priority_mutex);
    
    free(targ);
    return NULL;
}

int main(void) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    pthread_t threads[MAX_COMMANDS];
    int i;
    
    // Initialize logger
    logger_init();
    
    // Initialize hash table
    hash_table_init(&g_table);
    
    // Read commands.txt
    fp = fopen("commands.txt", "r");
    if (fp == NULL) {
        perror("Failed to open commands.txt");
        return 1;
    }
    
    num_commands = 0;
    while (fgets(line, sizeof(line), fp) != NULL && num_commands < MAX_COMMANDS) {
        if (parse_command(line, &commands[num_commands])) {
            num_commands++;
        }
    }
    fclose(fp);
    
    // Create threads for each command
    // Threads execute in priority order (0, 1, 2, ...)
    current_priority = 0;
    threads_finished = 0;
    total_threads = num_commands;
    
    for (i = 0; i < num_commands; i++) {
        thread_arg_t *arg = (thread_arg_t*)malloc(sizeof(thread_arg_t));
        arg->thread_id = i;
        arg->priority = commands[i].priority;
        arg->cmd = &commands[i];
        
        if (pthread_create(&threads[i], NULL, thread_func, arg) != 0) {
            perror("Failed to create thread");
            free(arg);
        }
    }
    
    // Wait for all threads to complete
    for (i = 0; i < num_commands; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Execute final PRINT command (required even if last command was PRINT)
    int count;
    hashRecord **records = hash_table_get_all_sorted(&g_table, &count);
    printf("Current Database:\n");
    if (count > 0) {
        for (i = 0; i < count; i++) {
            printf("%u,%s,%u\n", records[i]->hash, records[i]->name, records[i]->salary);
            free(records[i]); // Free the copied record
        }
        free(records); // Free the array
    }
    
    // Cleanup
    hash_table_destroy(&g_table);
    logger_close();
    
    return 0;
}
