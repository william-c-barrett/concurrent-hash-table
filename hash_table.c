#include "hash_table.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

void hash_table_init(hashTable *table) {
    table->head = NULL;
    rwlock_init(&table->lock);
}

void hash_table_set_priority(hashTable *table, int priority) {
    rwlock_set_priority(&table->lock, priority);
}

void hash_table_destroy(hashTable *table) {
    hashRecord *current = table->head;
    hashRecord *next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    
    rwlock_destroy(&table->lock);
}

int hash_table_insert(hashTable *table, const char *name, uint32_t salary) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    
    rwlock_acquire_write(&table->lock);
    
    // Check for duplicate
    hashRecord *current = table->head;
    while (current != NULL) {
        if (current->hash == hash) {
            rwlock_release_write(&table->lock);
            return -1; // Duplicate
        }
        current = current->next;
    }
    
    // Insert new record at the beginning
    hashRecord *new_record = (hashRecord*)malloc(sizeof(hashRecord));
    if (new_record == NULL) {
        rwlock_release_write(&table->lock);
        return -1;
    }
    
    new_record->hash = hash;
    strncpy(new_record->name, name, 49);
    new_record->name[49] = '\0'; // Ensure null termination
    new_record->salary = salary;
    new_record->next = table->head;
    table->head = new_record;
    
    rwlock_release_write(&table->lock);
    return 0; // Success
}

int hash_table_delete(hashTable *table, const char *name) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    
    rwlock_acquire_write(&table->lock);
    
    hashRecord *current = table->head;
    hashRecord *prev = NULL;
    
    while (current != NULL) {
        if (current->hash == hash) {
            if (prev == NULL) {
                table->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            rwlock_release_write(&table->lock);
            return 0; // Success
        }
        prev = current;
        current = current->next;
    }
    
    rwlock_release_write(&table->lock);
    return -1; // Not found
}

int hash_table_update(hashTable *table, const char *name, uint32_t new_salary, uint32_t *old_salary) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    
    rwlock_acquire_write(&table->lock);
    
    hashRecord *current = table->head;
    while (current != NULL) {
        if (current->hash == hash) {
            if (old_salary != NULL) {
                *old_salary = current->salary;
            }
            current->salary = new_salary;
            rwlock_release_write(&table->lock);
            return 0; // Success
        }
        current = current->next;
    }
    
    rwlock_release_write(&table->lock);
    return -1; // Not found
}

hashRecord* hash_table_search(hashTable *table, const char *name) {
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    
    rwlock_acquire_read(&table->lock);
    
    hashRecord *current = table->head;
    hashRecord *result = NULL;
    
    while (current != NULL) {
        if (current->hash == hash) {
            // Create a copy of the record to return (since we'll release the lock)
            result = (hashRecord*)malloc(sizeof(hashRecord));
            if (result != NULL) {
                *result = *current;
                result->next = NULL; // Don't copy the linked list
            }
            break;
        }
        current = current->next;
    }
    
    rwlock_release_read(&table->lock);
    return result;
}

// Comparison function for qsort
static int compare_hash(const void *a, const void *b) {
    hashRecord *rec_a = *(hashRecord**)a;
    hashRecord *rec_b = *(hashRecord**)b;
    
    if (rec_a->hash < rec_b->hash) return -1;
    if (rec_a->hash > rec_b->hash) return 1;
    return 0;
}

hashRecord** hash_table_get_all_sorted(hashTable *table, int *count) {
    // First, count the records
    int num_records = 0;
    hashRecord *current = table->head;
    
    rwlock_acquire_read(&table->lock);
    
    while (current != NULL) {
        num_records++;
        current = current->next;
    }
    
    if (num_records == 0) {
        rwlock_release_read(&table->lock);
        *count = 0;
        return NULL;
    }
    
    // Allocate array of pointers
    hashRecord **records = (hashRecord**)malloc(num_records * sizeof(hashRecord*));
    if (records == NULL) {
        rwlock_release_read(&table->lock);
        *count = 0;
        return NULL;
    }
    
    // Copy record data while holding the lock
    current = table->head;
    int i = 0;
    while (current != NULL) {
        records[i] = (hashRecord*)malloc(sizeof(hashRecord));
        if (records[i] == NULL) {
            // Free already allocated records and array
            rwlock_release_read(&table->lock);
            for (int j = 0; j < i; j++) {
                free(records[j]);
            }
            free(records);
            *count = 0;
            return NULL;
        }
        *records[i] = *current; // Copy the entire record
        records[i]->next = NULL; // Don't copy the linked list
        i++;
        current = current->next;
    }
    
    rwlock_release_read(&table->lock);
    
    // Sort by hash
    qsort(records, num_records, sizeof(hashRecord*), compare_hash);
    
    *count = num_records;
    return records;
}
