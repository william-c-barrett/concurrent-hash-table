#include "hash.h"

// Jenkins's one-at-a-time hash function
// Reference: https://en.wikipedia.org/wiki/Jenkins_hash_function
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
    uint32_t hash = 0;
    size_t i;
    
    for (i = 0; i < length; i++) {
        hash += key[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    
    return hash;
}
