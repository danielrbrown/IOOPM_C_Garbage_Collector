#include <string.h>
//#include "../gc.h"
#include "../header/header.h"
#include "../allocation_map/allocation_map.h"
#include "traverse_and_copy.h"
#include "../page/page.h"
#include "../page_map/page_map.h"
#include "../allocate/allocate.h"

static header_t *get_object_header(void *object)
{
    return (header_t *) ((char *) object - sizeof(header_t));
}

static page_t *first_free_passive_page(heap_t * h, size_t bytes)
{
    int no_pages = h_page_amount(h);
    page_t *first_page_address = h_first_page(h);
    for (int i = 0; i < no_pages; i++) {
        uint64_t page_address_int =
            (uint64_t) ((char *) first_page_address +
                        i * (p_metadata_size() + PAGESIZE));
        page_t *page_address = (void *) page_address_int;
        // If the page is passive and next_free_adress + bytes is less than the next page_address
        if (!pm_page_is_active(h, page_address)
            && p_free_space(page_address) >= bytes) {
            return page_address;
        }
    }

    // If no passive pages have enough space: return null
    return NULL;
}

static void *copy_object_and_return_pointer_to_new(heap_t * heap,
                                                   void **old_object,
                                                   size_t size)
{
    header_t *old_header = get_object_header(*old_object);
    void *new_object =
        allocate(heap, first_free_passive_page, *old_header, size);
    memcpy(new_object, *old_object, size - sizeof(header_t));
    am_set_allocation(heap, *old_object, false);
    *old_header = create_header_with_forwarding_pointer(new_object);

    return new_object;
}

static size_t get_object_size(const header_t * header)
{
    if (is_raw_layout_header(*header)) {
        return size_of_raw_allocation(*header);
    }
    else if (is_struct_layout_header(*header)) {
        return size_of_struct_allocation(*header);
    }
    else {
        return 0;
    }
}

static void manage_invalid_pointer(bool safe_pointer, int pointer_number)
{
    if (safe_pointer && pointer_number == 0) {  // Nothing has been copied yet, we are safe to return and continue
        return;
    }
    else {
        // If by chance we got a false pointer and then an interpreted "header" that gave us an offset 
        // that again by chance gave a false pointer into the heap, which was in turn allocated... (highly unlikely),
        // we will copy objects that are not safe to copy.
        assert(false
               &&
               "An invalid pointer has possibly resulted in an illegal write to memory.");
    }
}

static void traverse_children(heap_t * heap, void **root, void *origin,
                              bool safe_pointer, int *offset_array,
                              size_t number_of_offsets)
{
    for (size_t i = 0; i < number_of_offsets; i++) {
        void **child_address = (void **) ((char *) root + offset_array[i]);
        void *child = *child_address;

        if (child != NULL && child != origin && child != heap) {
            if (am_lookup(heap, child)) {
                traverse_and_copy(heap, (void **) child_address, origin,
                                  true);
            }
            else {
                // The interpreted "header" does not correspond with the allocation map, meaning that 
                // our "pointer" was not a real pointer.
                manage_invalid_pointer(safe_pointer, i);
            }
        }
    }
}

static void get_children(heap_t * heap, void **root, void *origin,
                         bool safe_pointer)
{
    header_t *header = get_object_header(root);
    int offset_array[MAX_ALLOC_SIZE] = { 0 };   // We assume that a struct cannot have more than 128 fields.
    size_t number_of_offsets = header_interpretation(*header, offset_array);
    traverse_children(heap, root, origin, safe_pointer, offset_array,
                      number_of_offsets);
}

void traverse_and_copy(heap_t * heap, void **root, void *origin,
                       bool safe_pointer)
{
    header_t *header = get_object_header(*root);

    if (is_forwarding_address_header(*header)) {
        *root = get_forwarding_pointer_from_header(*header);
    }
    else {
        if (!
            (is_raw_layout_header(*header)
             || is_struct_layout_header(*header))) {
            // We interpreted something as a pointer that turned out not to
            // be a pointer and returns the "unused header pattern"
            manage_invalid_pointer(safe_pointer, 0);
        }

        if (safe_pointer) {
            size_t allocation_size =
                get_object_size(header) + sizeof(header_t);
            *root =
                copy_object_and_return_pointer_to_new(heap, root,
                                                      allocation_size);
            header = get_object_header(*root);
        }
        else {
            page_t *page =
                h_get_page(heap, pm_index_from_pointer(heap, *root));
            p_set_unsafe_objects_flag(page, UNSAFE_OBJECTS);
        }

        if (is_struct_layout_header(*header)) {
            get_children(heap, *root, origin, safe_pointer);
        }
    }
}
