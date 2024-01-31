#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct entry entry_t;

struct entry
{
  ioopm_elem_t key;   // holds the key
  ioopm_elem_t value; // holds the value
  entry_t *next;      // points to the next entry (possibly NULL)
};

struct hash_table
{
  //entry_t buckets[No_Buckets];
  entry_t *buckets;
  size_t size;                               // Holds the total size of the hash-table
  ioopm_hash_function_t hash_func;           // hashfunction
  ioopm_hash_eq_function_t eq_hash_func;     // Ändrat jämför likhet
  ioopm_hash_comp_function_t comp_hash_func; // Ändrat, kollar vilken som är störst

  heap_t *heap;
};

/*------------------------------------------------
              END OF DEFINITIONS
---------------------------------------------------
*/

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function_t hash_func, ioopm_hash_comp_function_t comp_hash_func, ioopm_hash_eq_function_t eq_hash_func, heap_t *heap) // Ändrat
{
  //size_t size = No_Buckets * sizeof(entry_t);
  // entry_t *buckets = h_alloc_data(heap, size);
  ioopm_hash_table_t *ht = h_alloc_struct(heap, "*llll*"); // calloc(1, sizeof(ioopm_hash_table_t));
  
  ht->heap = heap;

  ht->buckets = h_alloc_struct(heap, "51*"); // h_alloc_data(heap, size); // buckets;
  ht->hash_func = hash_func; // Ändrat
  ht->comp_hash_func = comp_hash_func;
  ht->eq_hash_func = eq_hash_func;
  ht->size = 0;
  return ht;
}


void ioopm_entry_destroy(entry_t *bucket, ioopm_hash_table_t *ht)
{
  entry_t *current_entry = bucket;

  while (current_entry != NULL)
  {
    entry_t *tmp = current_entry;
    current_entry = current_entry->next;
    ht->size -= 1;
    // free(tmp);
  }
}

// Destroy function not needed with GC
// ==================================================
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
  int i = 0;
  while (i <= 16)
  {
    entry_t *current_bucket = &ht->buckets[i];
    ioopm_entry_destroy(current_bucket->next, ht);
    i++;
  }
  // free(ht);
}
// ==================================================

static entry_t *entry_create(ioopm_elem_t key, ioopm_elem_t value, entry_t *first_entry, heap_t *heap)
{
  entry_t *new_entry = h_alloc_struct(heap, "***");// calloc(1, sizeof(entry_t));

  new_entry->key = key;

  new_entry->value = value;

  new_entry->next = first_entry;

  return new_entry;
}

static entry_t *find_previous_entry_for_key(entry_t *bucket, ioopm_elem_t key, ioopm_hash_table_t *ht)
{
  entry_t *first_entry = bucket;

  entry_t *cursor = first_entry; // Removed if key < 0 return NULL
  while (cursor->next != NULL)   // cursor->next->key.int_elem >= key.int_elem
  {
    if (ht->comp_hash_func(cursor->next->key, key)) // Ta in en comp-funktion som jämför likhet. Annars jämför vi platser i minnet så även om det är samma sträng så blir det olika tolkning.
    {
      return cursor;
    }
    cursor = cursor->next;
  }
  return cursor;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, ioopm_elem_t key, ioopm_elem_t value)
{
  // removed if key < 0 return
  // Beräknar vilken bucket entryn kommer placeras i
  int bucket = (ht->hash_func(key)) % No_Buckets;
  // Letar efter en existerande entry för en key
  entry_t *entry = find_previous_entry_for_key(&ht->buckets[bucket % No_Buckets], key, ht);
  entry_t *next = entry->next;

  if (next != NULL && ht->eq_hash_func(next->key, key))
  {
    next->value = value;
  }
  else
  {
    entry->next = entry_create(key, value, next, ht->heap);
    ht->size += 1;
  }
}

ioopm_elem_t *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, ioopm_elem_t key)
{
  entry_t *tmp = find_previous_entry_for_key(&ht->buckets[ht->hash_func(key) % No_Buckets], key, ht); //&ht->buckets[key.int_elem % No_Buckets],
  entry_t *next = tmp->next;
  if (next && ht->eq_hash_func(next->key, key)) // next->key.int_elem == key.int_elem)
  {
    return (&next->value);
  }
  else
  {
    return NULL;
  }
}

ioopm_elem_t *ioopm_hash_table_remove(ioopm_hash_table_t *ht, ioopm_elem_t key)
{
  entry_t *prev_entry = find_previous_entry_for_key(&ht->buckets[ht->hash_func(key) % No_Buckets], key, ht);
  entry_t *entry_remove = prev_entry->next;
  ioopm_elem_t *entry_exist = ioopm_hash_table_lookup(ht, key);
  if (entry_exist)
  {

    prev_entry->next = entry_remove->next;
    // free(entry_remove);
    ht->size -= 1;
    ioopm_elem_t *elem_exist = entry_exist;
    return (elem_exist);
  }
  return NULL;
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  size_t ht_size = ht->size;
  return ht_size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
  return ioopm_hash_table_size(ht) == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
  int i = 0;
  while (i < No_Buckets)
  {
    entry_t *current_bucket = &ht->buckets[i];
    ioopm_entry_destroy(current_bucket->next, ht);
    current_bucket->next = NULL;
    i++;
  }
}

ioopm_elem_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
  int ht_size = ioopm_hash_table_size(ht);
  ioopm_elem_t value_array[ht_size];
  int arr_counter = 0;

  for (int arr_index = 0; arr_index < No_Buckets; arr_index++)
  {
    entry_t *bucket = &ht->buckets[arr_index];
    while (bucket->next != NULL)
    {
      bucket = bucket->next;
      value_array[arr_counter] = bucket->value;
      arr_counter++;
    }
  }

  ioopm_elem_t *result_array;
  if(sizeof(value_array) > 0) {
    result_array = h_alloc_data(ht->heap, sizeof(value_array)); // calloc(1, sizeof(value_array));
  }

  for (int i = 0; i < ht_size; i++)
  {
    result_array[i] = value_array[i];
  }

  return result_array;
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
  ioopm_list_t *key_list = ioopm_linked_list_create(ioopm_int_eq, ht->heap);
  for (int i = 0; i < No_Buckets; i++)
  {
    entry_t *bucket = &ht->buckets[i];
    entry_t *curr_entry = bucket->next;
    while (curr_entry != NULL)
    {
      ioopm_linked_list_append(key_list, curr_entry->key);
      curr_entry = curr_entry->next;
    }
  }
  return key_list;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
  for (int i = 0; i < No_Buckets; i++)
  {
    entry_t *current_bucket = &ht->buckets[i];
    entry_t *current_entry = current_bucket->next;
    while (current_entry != NULL)
    {
      if (!pred(current_entry->key, current_entry->value, arg))
      {
        return false;
      }

      current_entry = current_entry->next;
    }
  }
  return true;
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
  for (int i = 0; i < No_Buckets; i++)
  {
    entry_t *current_bucket = &ht->buckets[i];
    entry_t *current_entry = current_bucket->next;
    while (current_entry != NULL)
    {
      if (pred(current_entry->key, current_entry->value, arg))
      {
        return true;
      }

      current_entry = current_entry->next;
    }
  }
  return false;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{
  for (int i = 0; i < No_Buckets; i++)
  {
    entry_t *bucket = &ht->buckets[i];
    entry_t *entry = bucket->next;

    while (entry != NULL)
    {
      apply_fun(entry->key, &entry->value, arg);
      entry = entry->next;
    }
  }
}

static bool value_equiv(ioopm_elem_t key_ignored, ioopm_elem_t value, void *x)
{
  ioopm_elem_t *other_value_ptr = x;
  ioopm_elem_t other_value = *other_value_ptr;
  bool value_bool = false;
  if (strcmp(value.ptr_elem, other_value.ptr_elem) == 0)
  {
    value_bool = true;
  }
  return value_bool;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, ioopm_elem_t key)
{
  bool result = (ioopm_hash_table_lookup(ht, key)) != NULL;
  return result;
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, ioopm_elem_t value)
{

  bool result = ioopm_hash_table_any(ht, value_equiv, &value);
  return result;
}

heap_t *get_heap(ioopm_hash_table_t *ht) 
{
  return ht->heap;
}