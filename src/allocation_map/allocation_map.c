#include "allocation_map.h"
#include "../common/common.h"
#include "../header/header.h"
#include "../page_map/page_map.h"
#include "../bitvector/bitvector.h"

int am_index_from_ptr(page_t * page, void *ptr)
{
    long relative_address =
        (long) ((char *) ptr - (char *) p_start_address(page));
    int index = (int) relative_address / MIN_ALLOC_SIZE;
    return index;
}

void am_set_allocation(heap_t * heap, void *ptr, bool value)
{
    int page_index = pm_index_from_pointer(heap, ptr);
    assert(page_index >= 0 && page_index < h_page_amount(heap)
           && "Error: Page index out of range");

    page_t *page = h_get_page(heap, page_index);

    int index = am_index_from_ptr(page, ptr);

    assert(index >= 0 && index <= (2 * BITVECTOR_SIZE)
           && "Error: Allocation map index out of range");

    bitvector_t *alloc_map = p_get_alloc_map(page);

    bitvector_array_set_bit(alloc_map, index, value);
}

bool am_lookup(heap_t * heap, void *ptr)
{
    if (ptr < (void *) h_first_page(heap)
        || ptr > (void *) ((char *) heap + h_total_bytes(heap))) {
        assert(false
               && "ERROR: Lookup pointer not within allocated heap space");
    }
    assert((uint64_t) ptr % WORD_SIZE == 0
           && "ERROR: Lookup pointer is not word-aligned");

    int page_index = pm_index_from_pointer(heap, ptr);
    page_t *page = h_get_page(heap, page_index);
    bitvector_t *allocation_map = p_get_alloc_map(page);
    int allocation_index = am_index_from_ptr(page, ptr);
    return bitvector_array_read_bit(allocation_map, allocation_index);
}

void am_clear(page_t * page)
{
    bitvector_t *allocation_map = p_get_alloc_map(page);
    allocation_map[0] = (uint64_t) 0;
    allocation_map[1] = (uint64_t) 0;
}
