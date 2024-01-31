#pragma once
#include "linked_list.h"
#include "common.h"
#include <assert.h>

/**
 * @file iterator.h
 * @author Einar Johansson, Simon Eriksson
 * @date 3 Okt 2022
 * @brief Iterator for iterating over elements in a linked list.
 * 
 * Iterate over a linked list. 
 * Does not support insertion or removal using the iterator!
 * Does not support iterating on an empty list!
 * 
 * @see https://uppsala.instructure.com/courses/68435/assignments/130155
 */

typedef struct iter ioopm_list_iterator_t;

/// @brief Create an iterator for a given list
/// @param list the list to be iterated over
/// @return an iteration positioned at the start of list
ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list);

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if there is at least one more element 
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step
/// @param iter the iterator
/// @return Successful if the iterator has a next element. If it does, value = 
/// the next element, otherwise Failure is returned.
ioopm_option_t ioopm_iterator_next(ioopm_list_iterator_t *iter);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator where the underlying list is not empty!
/// @return the current element
ioopm_elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter);
