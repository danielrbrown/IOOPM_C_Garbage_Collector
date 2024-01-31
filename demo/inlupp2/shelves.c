#include "shelves.h"

ioopm_hash_table_t *ioopm_shelves_create(heap_t *heap)
{
  ioopm_hash_table_t *shelves = ioopm_hash_table_create(ioopm_string_sum_hash, ioopm_shelf_eq, ioopm_string_eq, heap); 
  return(shelves);
}

void ioopm_merch_insert_location(ioopm_list_t *shelf_locations, ioopm_shelf_t *shelf)
{
    ioopm_linked_list_prepend(shelf_locations, Shelf_elem(shelf));
}

ioopm_shelf_t *ioopm_shelf_create(char *shelf_name_input, uint32_t amount, heap_t *heap) 
{
  ioopm_shelf_t *shelf = h_alloc_struct(heap, "*i"); 
  shelf->shelf_name = shelf_name_input;
  shelf->amount = amount;
  return(shelf);

}

void ioopm_shelf_insert(ioopm_hash_table_t *shelves, char *shelf, ioopm_merch_t *merch)
{
  ioopm_hash_table_insert(shelves, Ptr_elem(shelf), Merch_elem(merch)); //Fel, detta ska vara merch.
}

void ioopm_shelf_remove(ioopm_hash_table_t *shelves, char *shelf_name) //Tar bort endast en shelf i ht. 
{
  ioopm_hash_table_remove(shelves, Ptr_elem(shelf_name)); //Ska inte fungera så utan ska ta bort value istället för key. Lagerplatserna ska ligga kvar men inte merchen. 
}

bool ioopm_is_shelf_free(ioopm_hash_table_t *shelves, char *shelf_name, ioopm_merch_t *merch)//MÅSTE VARA EN VALID KEY ANNARS SEGFAULTAR DET. 
{
    
  if(!ioopm_hash_table_has_key(shelves, Ptr_elem(shelf_name))) 
  {
    return true;
  }
  else
  {
    if(ioopm_shelf_contains(merch->locs, Ptr_elem(shelf_name)))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

void ioopm_shelves_destroy(ioopm_hash_table_t *shelves) //Nuke-function, tar bort allt. 
{
  ioopm_hash_table_destroy(shelves);
}

