/**
 * @file find_root_set.h
 * @author Fredrik Jäderblom
 * @author Erik Braathen
 * @brief Finds roots on stack pointing into heap, 
 *        then runs the traverse and copy algorithm for each root.
 * @date 2022-12-27
 */

// #ifndef __FIND_ROOT_SET__
// #define __FIND_ROOT_SET__
#pragma once

#include "../gc.h"

/**
 * @brief Checks if a pointer points into the heap
 *
 * @param heap a heap
 * @param p pointer potentially pointing into heap
 * @return true if pointer points into heap, otherwise false
 */
bool points_into_heap(heap_t * heap, const void *p);

/**
 * @brief Finds all the roots in the stack and traverses them.
 *
 * @param heap the heap to traverse
 * @return number of bytes collected
 */
void find_root_set(heap_t * heap);


/**
 * @brief killing off stack pointers
 * 
 * @param heap the heap
 * @param dbg_value a value to be written into every pointer into h on the stack
 */
void kill_stack_pointers(heap_t * heap, void *dbg_value);
// #endif
