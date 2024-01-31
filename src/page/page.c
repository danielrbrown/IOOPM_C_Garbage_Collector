#include "page.h"
#include "../common/common.h"
#include "../bitvector/bitvector.h"

struct page
{
    void *next_free_address;
    bitvector_t allocation_lower;
    bitvector_t allocation_upper;
    bool contains_unsafe_objects;
};

size_t p_metadata_size()
{
    return sizeof(page_t);
}

void *p_start_address(page_t * p)
{
    return (void *) ((char *) p + sizeof(page_t));
}

void *p_next_free(page_t * p)
{
    return p->next_free_address;
}

int p_free_space(page_t * page)
{
    unsigned long start = (unsigned long) p_start_address(page);
    unsigned long bump_ptr = (unsigned long) p_next_free(page);
    return (int) (PAGESIZE - (bump_ptr - start));
}

bitvector_t *p_get_alloc_map(page_t * p)
{
    return (bitvector_t *) & (p->allocation_lower);
}

void p_set_next_free_address(page_t * p, void *address)
{
    p->next_free_address = address;
}

void pages_init(void *start_address, int no_pages)
{
    page_t *page_header_address = (page_t *) start_address;
    for (size_t i = 0; i < no_pages; i++) {
        page_header_address =
            (page_t *) ((char *) start_address +
                        i * (PAGESIZE + sizeof(page_t)));

        page_t metadata = {
            .next_free_address =
                ((char *) page_header_address + sizeof(page_t)),
            .allocation_lower = 0,
            .allocation_upper = 0
        };

        memcpy(page_header_address, &metadata, sizeof(page_t)); // copies the struct onto the heap
    }
}

void p_reset(page_t * page)
{
    void *page_space = p_start_address(page);
    for (int i = 0; i < PAGESIZE; i++) {
        char *byte = (char *) page_space + i;
        *byte = '\0';
    }
}

void p_reset_bump_pointer(page_t * p)
{
    p->next_free_address = p_start_address(p);
}

bool p_contains_unsafe_objects(page_t * p)
{
    return p->contains_unsafe_objects;
}

void p_set_unsafe_objects_flag(page_t * p, bool status)
{
    p->contains_unsafe_objects = status;
}
