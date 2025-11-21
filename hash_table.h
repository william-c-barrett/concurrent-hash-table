#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include "rwlock.h"

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

typedef struct {
    hashRecord *head;
    rwlock_t lock;
} hashTable;

// Initialize hash table
void hash_table_init(hashTable *table);

// Set thread priority for logging
void hash_table_set_priority(hashTable *table, int priority);

// Destroy hash table and free all memory
void hash_table_destroy(hashTable *table);

// Insert a new record (returns 0 on success, -1 on duplicate)
int hash_table_insert(hashTable *table, const char *name, uint32_t salary);

// Delete a record (returns 0 on success, -1 if not found)
int hash_table_delete(hashTable *table, const char *name);

// Update salary (returns 0 on success, -1 if not found)
int hash_table_update(hashTable *table, const char *name, uint32_t new_salary, uint32_t *old_salary);

// Search for a record (returns pointer to record or NULL)
hashRecord* hash_table_search(hashTable *table, const char *name);

// Get all records sorted by hash (for printing)
hashRecord** hash_table_get_all_sorted(hashTable *table, int *count);

#endif // HASH_TABLE_H
