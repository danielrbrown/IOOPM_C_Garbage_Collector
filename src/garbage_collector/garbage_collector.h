#pragma once
#include "../gc.h"
#include "../allocation_map/allocation_map.h"

// #ifndef UNDEFINED_GARBAGE_COLLECTOR_H
// #define UNDEFINED_GARBAGE_COLLECTOR_H


/// @brief Iterates over all pages in the heap after all objects pointed to by
/// safe pointers have been copied to new pages. Pages are set to active or passive,
/// and the bump pointer and allocation map are reset according to the garbage collection
/// algorithm.
/// @param heap pointer to the heap
void garbage_collection(heap_t * heap);

// #endif //UNDEFINED_GARBAGE_COLLECTOR_H
