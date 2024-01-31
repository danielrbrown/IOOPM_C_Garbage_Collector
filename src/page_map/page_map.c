#include "page_map.h"
#include "../page/page.h"
#include "../heap/heap.h"

int pm_index_from_pointer(heap_t * heap, void *ptr)
{
    long relative_address =
        (long) ((char *) ptr - (char *) p_start_address(h_first_page(heap)));
    int index = (int) relative_address / P_ALLOC_SIZE;
    return index;
}

void pm_set_page_to(heap_t * heap, int page_index, bool status)
{
    if (page_index < 0 || page_index >= h_page_amount(heap)) {
        assert(false && "ERROR: Page index out of range");
    }
    bitvector_t *pagemap = h_get_pagemap(heap);
    bitvector_array_set_bit(pagemap, page_index, status);
}

bool pm_get_page_status(heap_t * heap, int page_index)
{
    if (page_index < 0 || page_index >= h_page_amount(heap)) {
        assert(false && "ERROR: Page index out of range");
    }
    bitvector_t *pagemap = h_get_pagemap(heap);

    return bitvector_array_read_bit(pagemap, page_index);
}

bool pm_page_is_active(heap_t * heap, page_t * page)
{
    return pm_get_page_status(heap, h_get_page_index(heap, page));
}

float pm_get_memory_pressure(heap_t * heap)
{
    bitvector_t *pagemap = h_get_pagemap(heap);
    size_t array_length = h_get_pagemap_array_length(heap);
    size_t active_pages = bitvector_array_count_ones(pagemap, array_length);
    int no_pages = h_page_amount(heap);
    //Print the number of active pages
    float memory_pressure = ((float) active_pages) / (no_pages / 2);
    // printf("\nPRESSURE: %f, NO_PAGES: %d, ACTIVE PAGES: %lu\n", memory_pressure, no_pages, active_pages);
    assert(memory_pressure <= 1
           && "ERROR: Memory pressure cannot be greater than 1.0");
    assert(memory_pressure >= 0
           && "ERROR: Memory pressure cannot be less than 0");

    return memory_pressure;
}
