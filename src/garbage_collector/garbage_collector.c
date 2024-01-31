#include "garbage_collector.h"
#include "../page/page.h"
#include "../page_map/page_map.h"
/*
UPDATING PAGES AFTER TRAVERSE AND COPY

All pages containing objects directly pointed to by the root set should have had the
`contains_unsafe_objects` flag set to True by the `traverse_and_copy` module before
coming here.

Iterate through each page on the heap:
 * Adjust its active/passive status
 * Reset its bump pointer if changed from active to passive
 * Reset its allocation map if changed from active to passive

1. Active pages with unsafe objects are left untouched
     (Page ACTIVE, Bump pointer ACTIVE, Allocation map ACTIVE)
2. Active pages with only safe objects are set to passive and reset
     (Page PASSIVE, Bump pointer RESET, Allocation map RESET)
3. Passive pages whose bump pointer is already reset (already passive) are left untouched
     (Page PASSIVE, Bump pointer INACTIVE, Allocation map EMPTY)
4. Passive pages whose bump pointer does not point to the beginning of the page space
   are set to active. The bump pointer and allocation map are left untouched
     (Page ACTIVE, Bump pointer ACTIVE, Aloocation map ACTIVE)
*/

void garbage_collection(heap_t * heap)
{
    int no_pages = h_page_amount(heap);

    for (int page_index = 0; page_index < no_pages; page_index++) {
        page_t *page = h_get_page(heap, page_index);

        if (pm_get_page_status(heap, page_index)) {
            if (p_contains_unsafe_objects(page)) {
                p_set_unsafe_objects_flag(page, SAFE_OBJECTS);
            }
            else {
                p_reset_bump_pointer(page);
                pm_set_page_to(heap, page_index, PASSIVE);
                am_clear(page);
            }
        }
        else {
            if (p_next_free(page) != p_start_address(page)) {
                pm_set_page_to(heap, page_index, ACTIVE);
            }
        }
    }
}
