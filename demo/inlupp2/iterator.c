#include "iterator.h"

/**
 * @struct iter
 * @brief Iterate over an underlying linked list
 * @param current Pointer to current element in the underlying list. Is NULL if 
 * we have iterated over the entire list or the underlying list is empty. 
 * @param list Pointer to the underlying list.
 */
struct iter {
  ioopm_link_t *current;
  ioopm_list_t *list;
};

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{  
  ioopm_list_iterator_t *iter = h_alloc_struct(list->heap, "**");// calloc(1, sizeof(ioopm_list_iterator_t));

  iter->list = list;
  iter->current = list->first;

  return iter;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
  if (iter->current) 
  {
    return iter->current->next != NULL; 
  } 
  
  return false;  
}

ioopm_option_t ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
  if (iter->current->next)
  {
    iter->current = iter->current->next;
    return Success(iter->current->element);
  }
  return Failure();
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
  iter->current = iter->list->first;
}

ioopm_elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
  assert(iter->current);
  return iter->current->element;
}

// NOT NEEDED WITH GC
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
  // free(iter);
}
