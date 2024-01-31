#pragma once
#include <stdint.h>

#define PAGESIZE 2048           // 0x800 = 2048 , jämna multipler blir alltså 0x NNNN N800, N = hex num

#define MIN_ALLOC_SIZE 16       // Smallest allocation we can make on the heap in bytes
#define MAX_ALLOC_SIZE 2040     // Largest possible allocation (2040 excluding 8 byte header)
#define WORD_SIZE 8
#define ACTIVE true
#define PASSIVE false
#define UNSAFE_OBJECTS true
#define SAFE_OBJECTS false
#define HALF_WORD_SIZE 4

typedef uint64_t bitvector_t;
typedef uint64_t header_t;
