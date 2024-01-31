#include "allocate.h"
#include "../allocation_map/allocation_map.h"
#include "../page_map/page_map.h"
#include "../bitvector/bitvector.h"
#include "../common/common.h"
#include "../heap/heap.h"
#include "../page/page.h"
#include "../util/string_util.h"
typedef uint64_t header_t;

page_t *first_free_page(heap_t * h, size_t bytes)
{
    int no_pages = h_page_amount(h);
    page_t *first_page_address = h_first_page(h);
    for (int i = 0; i < no_pages; i++) {
        uint64_t page_address_int =
            (uint64_t) first_page_address + i * (p_metadata_size() +
                                                 PAGESIZE);
        page_t *page_address = (void *) page_address_int;
        // If the page is active and next_free_adress + bytes is less than the the next page_address
        if (pm_page_is_active(h, page_address)
            && p_free_space(page_address) >= bytes) {
            return page_address;
        }
    }

    // If no active pages has enough space: return null
    return NULL;
}

static page_t *activate_and_get_a_passive_page(heap_t * h)
{
    int no_pages = h_page_amount(h);
    page_t *page_address = h_first_page(h);
    for (int i = 0; i < no_pages; i++) {
        page_t *current_page_address =
            (page_t *) ((uint64_t) page_address +
                        (uint64_t) (i * (p_metadata_size() + PAGESIZE)));
        // If the page is active and next_free_adress + bytes is less than the the next page_address
        if (!pm_page_is_active(h, current_page_address)) {
            p_reset(current_page_address);
            pm_set_page_to(h, h_get_page_index(h, current_page_address),
                           true);
            return current_page_address;
        }
    }

    assert(false && "ERROR: no passive pages to activate");
}

static page_t *get_page_for_new_allocation(heap_t * h, size_t bytes)
{
    page_t *page = first_free_page(h, bytes);
    if (page == NULL) {
        if (pm_get_memory_pressure(h) >= h_gc_threshold(h)) {
            h_gc(h);
        };
        page = activate_and_get_a_passive_page(h);
    }
    return page;
}

void *allocate(heap_t * heap, page_finder * page_finding_function,
               header_t header, size_t allocation_size)
{
    page_t *page = page_finding_function(heap, allocation_size);

    header_t *adress_for_header = (header_t *) p_next_free(page);
    *adress_for_header = header;

    void *adress_for_object =
        (void *) ((unsigned long) p_next_free(page) +
                  (unsigned long) sizeof(header_t));

    void *new_next_free_address =
        (void *) ((uint64_t) p_next_free(page) + (uint64_t) allocation_size);
    p_set_next_free_address(page, new_next_free_address);

    am_set_allocation(heap, adress_for_object, true);

    return adress_for_object;
}

void *allocate_new(heap_t * heap, header_t header, size_t allocation_size)
{
    return allocate(heap, get_page_for_new_allocation, header,
                    allocation_size);
}
