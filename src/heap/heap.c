#include "heap.h"
#include "../page/page.h"
#include "../common/common.h"
#include "../bitvector/bitvector.h"

struct heap
{
    page_t *first_page;
    size_t free_bytes;          // den beräknade ytan som vi får allokera i
    size_t total_bytes;         // den riktiga storleken av hela heapen
    bool unsafe_stack;
    float gc_threshold;
    int no_pages;               // totala antalet pages som vi har i heapen
    bitvector_t *pagemap;
};

void h_change_unsafe_stack(heap_t * heap, bool unsafe_stack)
{
    heap->unsafe_stack = unsafe_stack;
}

page_t *h_first_page(heap_t * h)
{
    return h->first_page;
}

size_t h_free_bytes(heap_t * h)
{
    return h->free_bytes;
}

size_t h_total_bytes(heap_t * h)
{
    return h->total_bytes;
}

size_t h_metadata_size()
{
    return sizeof(heap_t);
}

int h_page_amount(heap_t * h)
{
    return h->no_pages;
}

bool h_unsafe_stack(heap_t * h)
{
    return h->unsafe_stack;
}

float h_gc_threshold(heap_t * h)
{
    return h->gc_threshold;
}

bitvector_t *h_get_pagemap(heap_t * h)
{
    return h->pagemap;
}

page_t *h_get_page(heap_t * h, int page_index)
{
    if (page_index < 0 || page_index >= h_page_amount(h)) {
        assert(false && "ERROR: Page index out of range");
    }
    return (page_t *) (((char *) h_first_page(h)) +
                       (PAGESIZE + p_metadata_size()) * page_index);
}

int h_get_page_index(heap_t * h, page_t * p)
{
    int difference = (int) ((uint64_t) p - (uint64_t) h_first_page(h));
    int index = difference / P_ALLOC_SIZE;
    assert((difference % P_ALLOC_SIZE) == 0);
    assert(index < h_page_amount(h) && index >= 0);
    return index;
}

void h_set_metadata(int no_pages, size_t heap_alloc_size, bool unsafe_stack,
                    float gc_threshold, heap_t * heap_address,
                    size_t pagemap_size)
{
    heap_t metadata = {
        .no_pages = no_pages,
        .free_bytes = no_pages * PAGESIZE,      // kommer ändras under körning
        .total_bytes = heap_alloc_size, // den statiska storleken på allokeringen
        .unsafe_stack = unsafe_stack,
        .gc_threshold = gc_threshold,
        .first_page =
            (page_t *) ((char *) heap_address + h_metadata_size() +
                        pagemap_size),
        .pagemap = (bitvector_t *) (char *) heap_address + h_metadata_size()
    };

    memcpy(heap_address, &metadata, h_metadata_size());
}

size_t h_get_pagemap_array_length(heap_t * h)
{
    return ceil((float) h_page_amount(h) / BITVECTOR_SIZE);
}
