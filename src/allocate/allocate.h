#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "../heap/heap.h"
#include "../page/page.h"
#include "../header/header.h"

typedef page_t *page_finder(heap_t * heap, size_t bytes);

/// @brief Finds first free page
/// @param h the heap
/// @param bytes size of object
/// @return The first free page that fits object of size bytes on heap h
page_t *first_free_page(heap_t * h, size_t bytes);

/// @brief Allocates a space of given size for an object with an header
/// @param heap The heap where the allocation will be made
/// @param header The appropriate header for what will be stored in the allocated space
/// @param page_finding_function Function that returns a page that satisfies some requirement
/// @param allocation_size The size of the data that will be stored in the allocation
/// @return Pointer to the allocated space
void *allocate(heap_t * heap, page_finder * page_finding_function,
               header_t header, size_t allocation_size);

/// @brief Calls allocate with page finding function that finds either an active page with enough space
///        or activates a passive one.
void *allocate_new(heap_t * heap, header_t header, size_t allocation_size);
