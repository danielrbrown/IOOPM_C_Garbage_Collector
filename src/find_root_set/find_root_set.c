#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>

#include "find_root_set.h"
#include "../bitvector/bitvector.h"
#include "../allocation_map/allocation_map.h"
#include "../traverse_and_copy/traverse_and_copy.h"
#include "../header/header.h"


#define Dump_registers()                        \
  jmp_buf env;                                  \
  if (setjmp(env)) abort();                     \

/**
 * @brief Prints whether the stack grows upwards or downwards
 *
 * @param main_local_addr the local address of main
 */


static header_t *get_object_header(void *object)
{
    return (header_t *) (((char *) object) - sizeof(header_t));
}

/**
 * @brief Checks if a pointer is secure
 *
 * @param heap a heap
 * @param root a potentially secure pointer
 * @return true if pointer is secure, otherwise false
 */
static bool secure_pointer(heap_t * heap, void *root)
{
    return am_lookup(heap, root);
}

/**
 * @brief Checks if a pointer points into the heap
 *
 * @param heap a heap
 * @param p pointer potentially pointing into heap
 * @return true if pointer points into heap, otherwise false
 */
bool points_into_heap(heap_t * heap, const void *p)
{
    void *data_start = (void *) p_start_address(h_first_page(heap));
    void *data_end = (void *) ((char *) heap + h_total_bytes(heap));

    // Read three lsb, if they are 000, then it is a word aligned pointer to the heap
    bitvector_t bitvector = (bitvector_t) p;

    bool is_word_aligned = !(bitvector_read_bit(bitvector, 0)
                             || bitvector_read_bit(bitvector, 1)
                             || bitvector_read_bit(bitvector, 2));
    bool within_scope = (p >= data_start && p <= data_end);

    return within_scope && is_word_aligned;
}

/**
 * @brief Scans the stack for pointers to the heap
 *        and traverses and copies them if they are seen as secure.
 *
 * @param heap the heap
 * @return The amount of memory traversed and copied
 */
static void scan_stack(heap_t * heap, bool kill_stack_pointers,
                       void *dbg_value)
{
    char *stack_ptr = __builtin_frame_address(0);
    extern char **environ;
    char *stack_start = (char *) environ;

    for (; stack_ptr < stack_start; stack_ptr += sizeof(uintptr_t)) {
        void *root = (void *) *(uintptr_t *) stack_ptr;

        if (points_into_heap(heap, root)
            && (secure_pointer(heap, root)
                || is_forwarding_address_header(*get_object_header(root)))) {
            // check whether stack pointers should be killed, but without killing stack-pointer to the heap
            if (kill_stack_pointers) {
                // change the stack pointer to dbg_value
                *(uintptr_t *) stack_ptr = (uintptr_t) dbg_value;
            }
            else {
                traverse_and_copy(heap, &root, root, !h_unsafe_stack(heap));
                //Change stack pointer to root
                *(uintptr_t *) stack_ptr = (uintptr_t) root;
            }
        }
    }
}

void find_root_set(heap_t * heap)
{
    Dump_registers();
    scan_stack(heap, false, NULL);
}

void kill_stack_pointers(heap_t * heap, void *dbg_value)
{
    Dump_registers();
    scan_stack(heap, true, dbg_value);
}
