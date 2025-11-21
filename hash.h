#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <stddef.h>

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);

#endif // HASH_H
