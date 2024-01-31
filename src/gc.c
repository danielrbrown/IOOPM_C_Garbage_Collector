#include "gc.h"
#include "header/header.h"
#include "header/format_string/format_string.h"
#include "bitvector/bitvector.h"
#include "allocation_map/allocation_map.h"
#include "page_map/page_map.h"
#include "allocate/allocate.h"
#include "find_root_set/find_root_set.h"
#include "garbage_collector/garbage_collector.h"


/// @brief Calculate the size of a pagemap in bytes for a given number of pages
/// @param no_pages The number of pages that must fit into the pagemap
/// @return The number of bytes required for the pagemap
static size_t calculate_pagemap_size(size_t no_pages)
{
    if (no_pages % BITVECTOR_SIZE == 0) {
        return (no_pages / BITVECTOR_SIZE) * sizeof(bitvector_t);
    }
    else {
        return ((no_pages / BITVECTOR_SIZE) + 1) * sizeof(bitvector_t);
    }
}

/// @brief Calculates the number of pages that will fit into a heap
/// @param allocated_bytes The total number of bytes given by the client for the entire heap space
/// @return The number of pages that will fit into the heap
static size_t calculate_no_pages(size_t allocated_bytes)
{
    size_t bytes_minus_metadata = allocated_bytes - h_metadata_size();
    size_t page_alloc_size = PAGESIZE + p_metadata_size();

    // Calculate the number of pages that will fit into an allocated space with corresponding pagemap
    // to a good approximation, either the exact value or one minus the exact value.
    // The equation below was derived using a system of equations.
    size_t no_pages =
        (size_t) (8 * (bytes_minus_metadata - sizeof(bitvector_t))) /
        ((8 * page_alloc_size) + 1);

    // Increment the number of pages if the calculation above returned an approximation one less than the actual number:

    // Calculate the extra space available after the approximated number of pages and pagemap have been created
    // to check if we need to increment
    size_t extra_space =
        bytes_minus_metadata - (page_alloc_size * no_pages) -
        calculate_pagemap_size(no_pages);

    // Increment if:
    //    1. The extra space is great than or equal to the size of one page allocation and there is room in the pagemap
    //    2. The pagemap is full but there is space for exactly one extra page and an addtional pagemap bitvector
    if ((extra_space >= page_alloc_size && no_pages % BITVECTOR_SIZE != 0) ||
        (extra_space == page_alloc_size + 8
         && no_pages % BITVECTOR_SIZE == 0)) {
        no_pages++;
    }

    return no_pages;
}

/// @brief Initialise the heap's pagemap to be all zeroes
/// @param heap pointer to the heap
static void initialise_pagemap_to_zeroes(heap_t * heap)
{
    bitvector_t *pagemap = h_get_pagemap(heap);
    int no_pages = h_page_amount(heap);

    for (int i = 0; i < no_pages; i++) {
        bitvector_array_set_bit(pagemap, i, false);
    }
}

void h_delete(heap_t * h)
{
    free(h);
}

void h_delete_dbg(heap_t * h, void *dbg_value)
{
    kill_stack_pointers(h, dbg_value);

    h_delete(h);
}

heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
    if (gc_threshold < 0 || gc_threshold > 1) {
        assert(false
               &&
               "ERROR: Threshold must not be less than 0 or larger than 1.0");
    }

    size_t page_alloc_size = PAGESIZE + p_metadata_size();

    int no_pages = calculate_no_pages(bytes);

    size_t pagemap_size = calculate_pagemap_size(no_pages);

    // detta säkerställer tillräcklig plats för TVÅ pages (en active, en passive)
    int min_heap_size =
        page_alloc_size * 2 + h_metadata_size() + pagemap_size;

    if (bytes < min_heap_size) {
        assert(false
               && "ERROR: h_init called with too few bytes for two pages");
    }

    size_t heap_alloc_size =
        h_metadata_size() + pagemap_size + (no_pages * page_alloc_size);

    if (heap_alloc_size < min_heap_size || heap_alloc_size > bytes) {
        assert(false && "ERROR: heap_alloc_size calculated incorrectly");
    }

    heap_t *heap_address = calloc(1, heap_alloc_size);

    h_set_metadata(no_pages, heap_alloc_size, unsafe_stack, gc_threshold,
                   heap_address, pagemap_size);

    pages_init(h_first_page(heap_address), no_pages);

    initialise_pagemap_to_zeroes(heap_address);

    return heap_address;
}

size_t h_avail(heap_t * heap)
{
    size_t available = 0;
    int no_pages = h_page_amount(heap);
    for (int i = 0; i < no_pages; i++) {
        available += (size_t) p_free_space(h_get_page(heap, i));
    }
    return available;
}

static size_t calculated_alloc_size(size_t bytes)
{
    if (bytes <= 0) {
        return 0;
    }
    else if (bytes < WORD_SIZE) {
        return MIN_ALLOC_SIZE;
    }
    else {
        assert(bytes <= MAX_ALLOC_SIZE && "Error: Allocation too large");
        int blocks = 0;
        if (bytes % WORD_SIZE == 0) {
            blocks = (bytes / WORD_SIZE);
        }
        else {
            blocks = 1 + (bytes / WORD_SIZE);
        }
        return (size_t) (blocks * WORD_SIZE + sizeof(header_t));
    }
}

static size_t size_of_user_data_from_header(header_t * header,
                                            size_t
                                            *format_string_allocation_size)
{
    size_t alloc_size = 0;
    if (is_raw_layout_header(*header)) {
        alloc_size = size_of_raw_allocation(*header);
    }
    else if (is_struct_layout_header(*header)) {
        alloc_size = size_of_struct_allocation(*header);
    }
    else if (is_format_string_header(*header)) {
        char buffer[MAX_ALLOC_SIZE] = { '\0' };
        format_string_expander((char *) *header, buffer);
        alloc_size =
            expanded_format_string_required_space_for_data_alloc(buffer);
        // to account for the allocated format string metadata
        // +1 for the string termination char
        *format_string_allocation_size +=
            calculated_alloc_size(strlen((char *) *header) + 1) -
            sizeof(header_t);
    }
    else if (is_forwarding_address_header(*header)) {
        header_t temp = *header;
        change_header_type_to(&temp, 0);        // sets the LSBs to 0 so we can use it as a pointer
        alloc_size =
            size_of_user_data_from_header(&temp,
                                          format_string_allocation_size);
    }
    else {
        assert(false && "Error: Unexpected header type");
    }
    return alloc_size;
}

size_t h_used(heap_t * heap)
{
    header_t *next_header = NULL;
    page_t *page = NULL;

    size_t used = 0;
    size_t alloc_size = 0;
    size_t size_of_allocated_format_strings = 0;

    int uncounted_allocs = 0;
    int no_pages = h_page_amount(heap);

    for (int i = 0; i < no_pages; i++) {
        if (pm_get_page_status(heap, i)) {
            page = h_get_page(heap, i);
            next_header = p_start_address(page);
            uncounted_allocs =
                bitvector_array_count_ones(p_get_alloc_map(page), 2);
            while (uncounted_allocs != 0) {
                alloc_size =
                    size_of_user_data_from_header(next_header,
                                                  &size_of_allocated_format_strings);

                if (!is_forwarding_address_header(*next_header)) {
                    //We do not count forwarded data as it has been moved and would
                    //then be counted twice.
                    uncounted_allocs--;
                    used += alloc_size;
                }
                next_header =
                    (header_t *) ((char *) next_header + alloc_size +
                                  sizeof(header_t));
            }
        }
    }
    return used - size_of_allocated_format_strings;
}

size_t h_gc(heap_t * h)
{
    size_t mem_before_gc = h_avail(h);

    find_root_set(h);

    // printf("\nRunning GC...\t");

    garbage_collection(h);

    size_t mem_after_gc = h_avail(h);
    size_t collected_mem = mem_after_gc - mem_before_gc;

    // printf("Collected: %lu bytes\n\n", collected_mem);

    return collected_mem;
}

size_t h_gc_dbg(heap_t * h, bool unsafe_stack)
{
    h_change_unsafe_stack(h, unsafe_stack);

    size_t collected_mem = h_gc(h);

    return collected_mem;
}

//---------- ALLOC ---------------

static bool new_page_is_passive_and_empty(heap_t * h, size_t bytes)
{
    return first_free_page(h, bytes) == NULL;
}

/// @brief Checks if gc should run
/// @param h the heap
/// @param page the page
/// @return true iff gc should run
static bool run_gc(heap_t * h, size_t size)
{
    if (new_page_is_passive_and_empty(h, size)) {       //Allocate happens on a new page
        return pm_get_memory_pressure(h) >= h_gc_threshold(h);  // Compare heaps threshold and memory pressure
    }
    return false;
}

static void *alloc_raw(heap_t * h, header_t header)
{
    size_t data_size = size_of_raw_allocation(header);
    int total_size = (data_size + sizeof(header_t));    // Total size of what to be allocated including header

    //if(run_gc(h, total_size)){
    //    h_gc(h);
    //}
    return allocate_new(h, header, total_size);

}

void *h_alloc_struct(heap_t * h, char *layout)
{
    header_t header = header_creation_layout(layout, h);

    if (is_raw_layout_header(header)) {
        return alloc_raw(h, header);
    }

    size_t size = size_of_struct_allocation(header) + sizeof(header_t);
    //if(run_gc(h, size)){
    //    h_gc(h);
    //}

    return allocate_new(h, header, size);
}


// Since the max allocation size is 2040, there will never be more than 4 digits
#define MAX_DIGITS 4

void *h_alloc_data(heap_t * h, size_t bytes)
{
    char int_string[MAX_DIGITS + 1];    // +1 for null character
    sprintf(int_string, "%i", (int) bytes);
    header_t header = header_creation_layout(int_string, h);
    return alloc_raw(h, header);
}
