#pragma once
#include "../gc.h"

/**
 * @brief traverse a root and copy objects object and its children to free memory space.
 * @param heap pointer to the heap
 * @param root root to be traversed
 * @param safe_pointer indicates whether the root pointer is to be considered safe
 */
void traverse_and_copy(heap_t * heap, void **root, void *origin,
                       bool safe_pointer);
