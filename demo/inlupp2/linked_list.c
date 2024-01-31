#include "linked_list.h"

/// @brief Creates and allocates memory for a link and links it into the given 
/// list. Updates the length of the list. 
/// @param list The list where the newly created link will be linked into.
/// @param value The value for the new link. 
/// @param next Pointer to next link.
/// @return Pointer to the new link
static ioopm_link_t *create_link(ioopm_list_t *list, ioopm_elem_t value, ioopm_link_t *next)
{
  ioopm_link_t *link = h_alloc_struct(list->heap, "**"); // calloc(1, sizeof(ioopm_link_t));

  link->element = value;
  link->next = next;

  // Om list är tom
  if (list->size == 0)
  {
    list->first = link;
    list->last = link;
  }

  list->size += 1;

  return link;
}

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq_fun, heap_t *heap)
{
  ioopm_list_t *ll = h_alloc_struct(heap, "***li"); // calloc(1, sizeof(ioopm_list_t));
  ll->heap = heap;
  ll->eq_fun = eq_fun;
  return ll;
}

/// @brief Free the memory allocated for link (doesn't free the value of link). 
/// Updates the length of list.
/// @param list Pointer to the list where link resides.
/// @param link Pointer to the link we want to free.
static void destroy_link(ioopm_list_t *list, ioopm_link_t *link)
{
  // Om vi tar bort sista elementet i listan
  if (list->size == 1)
  {
    list->first = NULL;
    list->last = NULL;
  }

  list->size -= 1;
  // free(link);
}

/// @brief Free the memory allocated for all links in the linked list.
/// @param list Pointer to the list where link resides.
/// @param link Pointer to the first link in list.
static void destroy_links(ioopm_list_t *list, ioopm_link_t *link)
{
  if (link)
  {
    destroy_links(list, link->next);
    destroy_link(list, link);
  }
}

void ioopm_linked_list_destroy(ioopm_list_t *list)
{
  if (list)
  {
    destroy_links(list, list->first);
    // free(list);
  }
}


void ioopm_linked_list_append(ioopm_list_t *list, ioopm_elem_t value)
{
  int length = list->size;
  ioopm_link_t *last = create_link(list, value, NULL);

  if (length > 0)
  {
    list->last->next = last;
  }
  list->last = last;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, ioopm_elem_t value)
{
  ioopm_link_t *first = create_link(list, value, list->first);
  list->first = first;
}

ioopm_option_t ioopm_linked_list_get(ioopm_list_t *list, uint32_t index)
{
  ioopm_link_t *entry = list->first;
  uint32_t i = 0;

  while (i <= index && entry)
  {
    if (i == index)
    {
      return Success(entry->element);
    }
    entry = entry->next;
    i++;
  }

  return Failure();
}

ioopm_option_t ioopm_linked_list_insert(ioopm_list_t *list, uint32_t index, ioopm_elem_t value)
{
  ioopm_link_t *prev = list->first;

  if (!prev && index == 0)
  {
    ioopm_link_t *new = create_link(list, value, NULL);
    list->first = new;
    list->last = new;
    return Success(new->element);
  }

  if (index == 0)
  {
    ioopm_linked_list_prepend(list, value);
    return Success(value);
  }

  if (index == list->size)
  {
    ioopm_linked_list_append(list, value);
    return Success(value); 
  }

  if (0 < index && index < list->size)
  {
    for (uint32_t i = 0; i < index; i++)
    {
      prev = prev->next;
    }
    ioopm_link_t *new = create_link(list, value, prev->next);
    prev->next = new;
    return Success(new->element);
  }

  return Failure();
}

uint32_t ioopm_linked_list_size(ioopm_list_t *list)
{
  return list->size;
}

void ioopm_linked_list_clear(ioopm_list_t *list)
{
  destroy_links(list, list->first);
}

ioopm_option_t ioopm_linked_list_remove(ioopm_list_t *list, uint32_t index)
{
  ioopm_link_t *current_link = list->first;
  // Är listan tom?
  if(!current_link) 
  {
    return Failure();
  }

  // Tar bort första elementet
  if (index == 0)
  {
    list->first = current_link->next;

    int value = current_link->element.int_elem;
    destroy_link(list, current_link);

    return Success(Int_elem(value));
  }

  // Om annat valid index
  uint32_t i = 0;
  if (index > 0 && index < list->size)
  {
    while(current_link)
    {
      if (i == index-1)
      {
        int value = current_link->next->element.int_elem;
        
        ioopm_link_t *next = current_link->next->next;
        
        destroy_link(list, current_link->next);
        
        current_link->next = next;

        if (next == NULL)
        {
          list->last = current_link;
        }

        return Success(Int_elem(value)); 
      }

      i++;
      current_link = current_link->next;
    }
  }
  return Failure();
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
  return list->size == 0;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
  ioopm_link_t *current_entry = list->first;
  uint32_t i = 0;

  while(current_entry)
  {
    if (prop(Int_elem(i), current_entry->element, extra))
    {
      return true;
    }
    i++;
    current_entry = current_entry->next;
  }
  return false;
}

bool ioopm_linked_list_contains(ioopm_list_t *list, ioopm_elem_t element)
{ 
  ioopm_link_t *current_entry = list->first;

  while(current_entry)
  {
    if (list->eq_fun(element, current_entry->element))
    {
      return true;
    }
    current_entry = current_entry->next;
  }
  return false;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
  ioopm_link_t *current_entry = list->first;
  uint32_t i = 0;

  while(current_entry)
  {
    if (!prop(Int_elem(i), current_entry->element, extra))
    {
      return false;
    }

    i++;
    current_entry = current_entry->next;
  }

  return !ioopm_linked_list_is_empty(list);
}

void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_function fun, void *extra)
{
  ioopm_link_t *current_link = list->first;
  uint32_t i = 0;
  while (current_link)
  {
    fun(Int_elem(i), &current_link->element, extra);
    i++;
    current_link = current_link->next;
  }
}

