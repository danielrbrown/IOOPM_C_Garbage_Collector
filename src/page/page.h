#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "../common/common.h"

#define P_ALLOC_SIZE (p_metadata_size()+PAGESIZE)       // 2048+sizeof(page_t)

/// The opaque data type holding a single page
typedef struct page page_t;

/// @brief Initializes pages starting from 'start_address' until 'no_pages' pages
/// have been initialized. This makes a 'page_t' every x bytes in the
/// allocated heap area starting from 'start_address'.
///
/// @param start_address pointer to where pages will start from
/// @param no_pages amount of pages to initialize
void pages_init(void *start_address, int no_pages);

/// @brief Gets the size of the page metadata (the page_t struct)
/// @return the size of metadata for a single page
size_t p_metadata_size();

/// @brief Gets the next free address within a page
/// @param p pointer to page struct
/// @return the next free address within a page
void *p_next_free(page_t * p);

/// @brief Gets the start address of the allocatable space in a page
/// @param p pointer to page struct
/// @return the start address of the allocatable space in a page
void *p_start_address(page_t * p);

/// @brief Gets the allocation map from a page
/// @param p pointer to page struct
/// @return the allocation map for the specific page
bitvector_t *p_get_alloc_map(page_t * p);

/// @brief Sets the next free address for a page
/// @param p Page where change will be made
/// @param address The new free address
void p_set_next_free_address(page_t * p, void *address);

/// @brief Calculates the amount of free space that can be used for allocation on a page
/// @param page The page
/// @return The amount of free space in bytes
int p_free_space(page_t * page);

/// @brief Sets all the bytes of a pages allocatable space to zero
/// @page The page which will be resetted
void p_reset(page_t * page);

/// @brief Reset a page's bump pointer to the beginning of the allocatable space
/// @param p pointer to the page
void p_reset_bump_pointer(page_t * p);

/// @brief Checks if a page contains unsafe objects
/// @param p pointer to the page to ne checked
/// @return True if the page contains unsafe objects, otherwise false
bool p_contains_unsafe_objects(page_t * p);

/// @brief Sets a page's unsafe objects flag
/// @param p pointer to the page to be set
/// @param status True if the page contains unsafe objects, otherwise false
void p_set_unsafe_objects_flag(page_t * p, bool status);
