#pragma once
#include <stdbool.h>
#include "../common/common.h"
#include "../heap/heap.h"

/// @brief Finds what page number a pointer is located at
/// @param heap pointer to the heap struct
/// @param ptr an arbitrary pointer on the heap
/// @return what page number (first page is 0) the pointer is located in
int pm_index_from_pointer(heap_t * heap, void *ptr);

/// @brief Set a particular page in a pagemap to passive or active
/// @param heap pointer to the heap that the pagemap maps
/// @param page_index the page to be set
/// @param status true if the page should be set to active, false if it should
/// be set to passive
void pm_set_page_to(heap_t * heap, int page_index, bool status);

/// @brief Read the status of a particular page (passive or active)
/// @param heap pointer to the heap where the pages are located
/// @param page_index index of the page to be read
/// @return true if the page is active, false if it is passive
bool pm_get_page_status(heap_t * heap, int page_index);

/// @brief Checks whether a page is active or not
/// @param h The heap where the page is located
/// @param page The page
/// @return true if the page is active, otherwise false
bool pm_page_is_active(heap_t * heap, page_t * page);

/// @brief Calculates the current memory pressure in the heap. 
/// @param heap pointer to the heap
/// @return the current memory pressure
/// NOTE: Pressure is based upon the number of pages that are active, not how many
/// bytes are allocated (as there will likely be unallocated bytes at the end of each
/// page that are not used). The total number of pages is first divided by 2 and rounded
/// downwards to the nearest integer. The pressure of *no more than half* of the pages 
/// is then calculated.
float pm_get_memory_pressure(heap_t * heap);
