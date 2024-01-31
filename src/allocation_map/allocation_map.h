#pragma once
#include <stdint.h>
#include "../heap/heap.h"
#include "../page/page.h"

/// @brief Translates a pointer into an index in the allocation map for a page
/// @param page which contains this pointer
/// @param ptr which we want to find the index of
/// @return the index of ptr in the allocation map of page
int am_index_from_ptr(page_t * page, void *ptr);

/// @brief Sets the bit in the allocation map that corresponds to the block
/// of a given pointer
/// @param heap pointer to the heap struct
/// @param ptr the pointer we have allocated
/// @param value true for set, false to clear
void am_set_allocation(heap_t * heap, void *ptr, bool value);

/// @brief Check if a block is allocated at the place in memory given by a pointer
/// @param heap pointer to the heap to be checked
/// @param ptr pointer to the address where an allocation will be checked
/// @return true if the block pointed to is active, otherwise false
bool am_lookup(heap_t * heap, void *ptr);

/// @brief Clear a given page's allocation map (set all bits to 0)
/// @param page the page whose allocation map will be cleared
void am_clear(page_t * page);
