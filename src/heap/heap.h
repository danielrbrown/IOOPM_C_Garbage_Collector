#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../bitvector/bitvector.h"
#include "../page/page.h"

/// The opaque data type holding all the heap data
typedef struct heap heap_t;

/// @brief Change the unsafe_stack bool in heap to the new unsafe_stack value
/// @param heap the heap
/// @param unsafe_stack the new unsafe_stack value
void h_change_unsafe_stack(heap_t * heap, bool unsafe_stack);

// -- GETTERS --

/// @brief Gets a pointer to the first page struct in the heap
/// @param h pointer to heap
/// @return a pointer to the first page struct in the heap h
page_t *h_first_page(heap_t * h);

/// @brief Gets the amount of free space in the heap in bytes
/// @param h pointer to heap
/// @return the amount of free space in the heap in bytes
size_t h_free_bytes(heap_t * h);

/// @brief Gets the total allocated size on disk of the heap. Does not change after init.
/// @param h pointer to heap
/// @return total allocated size of the heap
size_t h_total_bytes(heap_t * h);

/// @brief Gets the size of the heap metadata (the heap_t struct)
/// @return the size of the heaps metadata
size_t h_metadata_size();

/// @brief Gets the amount of pages a heap contains.
/// @param h pointer to heap struct
/// @return the number of pages in a heap h.
int h_page_amount(heap_t * h);

/// @brief Checks if heap is unsafe or not
/// @param h pointer to heap struct
/// @return the unsafe_stack flag
bool h_unsafe_stack(heap_t * h);

/// @brief Gets the threshold set for garbage collection
/// @param h pointer to heap
/// @return the threshold for garbage collection
float h_gc_threshold(heap_t * h);

/// @brief Gets a pointer to the heap's pagemap
/// @param h pointer to heap
/// @return a pointer to the heap's pagemap
bitvector_t *h_get_pagemap(heap_t * h);

/// @brief Gets the page at a given index
/// @param h pointer to the heap that contains the page
/// @param page_index the index of the page (counting from 0)
/// @return a pointer to the page at the given index
page_t *h_get_page(heap_t * h, int page_index);

/// @brief Sets the metadata for an allocated heap
void h_set_metadata(int no_pages, size_t heap_alloc_size, bool unsafe_stack,
                    float gc_threshold, heap_t * heap_address,
                    size_t pagemap_size);

/// @brief Finds what the index for the pagemap would be for a given page in a heap
/// @param h The heap
/// @param p The page
/// @return The index for the bit in the pagemap representing the given page 
int h_get_page_index(heap_t * h, page_t * p);

/// @brief Calculates the array length (number of bitvectors) of the pagemap
/// @param h pointer to the heap
/// @return the length (in number of bitvectors) of the pagemap array
size_t h_get_pagemap_array_length(heap_t * h);
