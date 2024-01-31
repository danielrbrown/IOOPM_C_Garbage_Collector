#include "warehouse.h"
#include "shelves.h"
ioopm_hash_table_t *ioopm_warehouse_create(heap_t *heap)
{
  ioopm_hash_table_t *wh = ioopm_hash_table_create(ioopm_string_sum_hash, ioopm_comp_string_func, ioopm_string_eq, heap);
  return wh;
}

void ioopm_warehouse_destroy(ioopm_hash_table_t *warehouse)
{
  // ioopm_hash_table_destroy(warehouse);
  warehouse = NULL;
}

ioopm_merch_t *ioopm_merch_create(char *name, char *desc, int price, heap_t *heap)
{
  ioopm_list_t *locs = ioopm_linked_list_create(ioopm_shelf_eq, heap);
  ioopm_merch_t *new_merch = h_alloc_struct(heap, "**iii*"); // calloc(1, sizeof(ioopm_merch_t));
  new_merch->name = name;
  new_merch->locs = locs;
  new_merch->desc = desc;
  new_merch->price = price;
  new_merch->total_stock = 0;
  new_merch->stock_in_cart = 0;
  return new_merch;
}

bool ioopm_merch_already_in_warehouse(ioopm_hash_table_t *wh, char *merch_name)
{
  return ioopm_hash_table_has_key(wh, Ptr_elem(merch_name));
}

void ioopm_merch_insert(ioopm_hash_table_t *wh, ioopm_merch_t *merch)
{
  if (!ioopm_merch_already_in_warehouse(wh, merch->name))
  {
    ioopm_hash_table_insert(wh, Ptr_elem(merch->name), Merch_elem(merch));
  }
}

void ioopm_merch_edit_price(ioopm_hash_table_t *wh, ioopm_merch_t *merch, int new_price)
{
  ioopm_elem_t *target_merch = ioopm_hash_table_lookup(wh, Ptr_elem(merch->name));
  ioopm_merch_t *merch_to_edit = target_merch->merch_elem;
  merch_to_edit->price = new_price;
}

void ioopm_merch_edit_name(ioopm_hash_table_t *wh, ioopm_merch_t *merch, char *new_name)
{
  ioopm_elem_t *target_merch = ioopm_hash_table_lookup(wh, Ptr_elem(merch->name));
  ioopm_merch_t *merch_to_edit = target_merch->merch_elem;
  ioopm_hash_table_remove(wh, Ptr_elem(merch->name));
  merch_to_edit->name = new_name;
  ioopm_hash_table_insert(wh, Ptr_elem(new_name), Merch_elem(merch));
}

void ioopm_merch_edit_desc(ioopm_hash_table_t *wh, ioopm_merch_t *merch, char *new_desc)
{
  ioopm_elem_t *target_merch = ioopm_hash_table_lookup(wh, Ptr_elem(merch->name));
  ioopm_merch_t *merch_to_edit = target_merch->merch_elem;
  merch_to_edit->desc = new_desc;
}

void ioopm_merch_edit(ioopm_hash_table_t *wh, ioopm_merch_t *merch, char *new_name, char *new_desc, int new_price, char flags)
{
  if (flags & EDIT_NAME)
  {
    ioopm_merch_edit_name(wh, merch, new_name);
  }
  if (flags & EDIT_DESC)
  {
    ioopm_merch_edit_desc(wh, merch, new_desc);
  }
  if (flags & EDIT_PRICE)
  {
    ioopm_merch_edit_price(wh, merch, new_price);
  }
}

bool ioopm_shelf_contains(ioopm_list_t *list, ioopm_elem_t element)
{
  ioopm_link_t *current_entry = list->first;

  while (current_entry)
  {
    if (list->eq_fun(element, Ptr_elem(current_entry->element.shelf_elem->shelf_name)))
    {
      return true;
    }
    current_entry = current_entry->next;
  }
  return false;
}

uint32_t ioopm_shelf_index(ioopm_merch_t *merch, char *shelf_name, heap_t *heap)
{
  ioopm_list_iterator_t *iter = ioopm_list_iterator(merch->locs);
  uint32_t counter = 0;
  while (strcmp(ioopm_iterator_current(iter).shelf_elem->shelf_name, shelf_name) != 0)
  {
    counter++;
    ioopm_iterator_next(iter);
  }
  ioopm_iterator_destroy(iter);
  return counter;
}

void ioopm_merch_replenish(ioopm_hash_table_t *shelves, ioopm_merch_t *merch, uint32_t amount, char *shelf_name)
{
  if (ioopm_shelf_contains(merch->locs, Ptr_elem(shelf_name)))
  {
    uint32_t index = ioopm_shelf_index(merch, shelf_name, get_heap(shelves));
    uint32_t tmp = (ioopm_linked_list_get(merch->locs, index).value.shelf_elem)->amount;
    ioopm_elem_t curr_shelf = ioopm_linked_list_get(merch->locs, index).value;
    ioopm_shelf_t *shelf = curr_shelf.shelf_elem;
    shelf->amount = amount + tmp;
    merch->total_stock += amount;
  }
  else
  {
    ioopm_shelf_t *new_shelf = ioopm_shelf_create(shelf_name, amount, get_heap(shelves));
    ioopm_merch_insert_location(merch->locs, new_shelf);
    ioopm_shelf_insert(shelves, shelf_name, merch);
    merch->total_stock += amount;
  }
}

static bool is_replenished(ioopm_elem_t key_ignored, ioopm_elem_t value, void *extra)
{
  if (value.merch_elem->total_stock > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool ioopm_merch_is_replenished(ioopm_hash_table_t *wh)
{
  if (ioopm_hash_table_any(wh, is_replenished, NULL))
  {
    return true;
  }
  else
  {
    return false;
  }
}

static bool merch_in_cart(ioopm_elem_t index, ioopm_elem_t value, void *extra)
{
  if (ioopm_hash_table_size(value.cart_elem->products) > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool ioopm_merch_in_cart_any(ioopm_cart_list_t *cart_list)
{
  if (ioopm_linked_list_any(cart_list->carts, merch_in_cart, NULL))
  {
    return true;
  }
  else
  {
    return false;
  }
}

static void shelf_destroy(ioopm_hash_table_t *shelves, ioopm_merch_t *merch)
{
  if (shelves != NULL)
  {
    for (int counter_index = 0; counter_index < merch->locs->size; counter_index++)
    {
      ioopm_elem_t current_shelf = ioopm_linked_list_get(merch->locs, counter_index).value;
      ioopm_shelf_t *shelf = current_shelf.shelf_elem;
      ioopm_shelf_remove(shelves, shelf->shelf_name);
    }
  }
  uint32_t num_of_locs = ioopm_linked_list_size(merch->locs);
  for (int j = 0; j < num_of_locs; j++)
  {
    ioopm_elem_t shelf_to_destroy = (ioopm_linked_list_get(merch->locs, j)).value;
    ioopm_shelf_t *shelf = shelf_to_destroy.shelf_elem;
    // free(shelf);
    shelf = NULL;
  }
}

static void shelf_spec_destroy(ioopm_hash_table_t *shelves, ioopm_merch_t *affected_merch, ioopm_shelf_t *shelf_to_destroy)
{
  ioopm_shelf_remove(shelves, shelf_to_destroy->shelf_name);
  uint32_t index = ioopm_shelf_index(affected_merch, shelf_to_destroy->shelf_name, get_heap(shelves));
  ioopm_linked_list_remove(affected_merch->locs, index);
  // shelf_to_destroy = NULL; // free(shelf_to_destroy);
}

static void merch_destroy(ioopm_hash_table_t *warehouse, ioopm_merch_t *merch)
{
  ioopm_linked_list_destroy(merch->locs);
  ioopm_hash_table_remove(warehouse, Ptr_elem(merch->name));
  // merch = NULL; // free(merch);
}

void ioopm_product_destroy(ioopm_hash_table_t *shelves, ioopm_hash_table_t *warehouse, ioopm_merch_t *merch)
{
  shelf_destroy(shelves, merch);
  merch_destroy(warehouse, merch);
}

ioopm_cart_list_t *ioopm_cart_list_create(heap_t *heap)
{
  ioopm_list_t *all_carts = ioopm_linked_list_create(ioopm_shelf_eq, heap);
  ioopm_cart_list_t *cart_list = h_alloc_struct(heap, "*ii"); // calloc(1, sizeof(ioopm_cart_list_t));

  cart_list->carts = all_carts;
  cart_list->next_cart_id = 0;
  cart_list->no_carts_removed = 0;
  return cart_list;
}

void ioopm_cart_create(ioopm_cart_list_t *cart_list)
{
  ioopm_hash_table_t *products = ioopm_hash_table_create(ioopm_string_sum_hash, ioopm_comp_string_func, ioopm_merch_eq, cart_list->carts->heap);
  ioopm_cart_t *cart =  h_alloc_struct(cart_list->carts->heap, "*i"); // calloc(1, sizeof(ioopm_cart_t));
  cart->products = products;
  cart->cart_id = cart_list->next_cart_id;
  cart_list->next_cart_id += 1;
  ioopm_linked_list_append(cart_list->carts, Cart_elem(cart));
}

static void cart_item_nuke(ioopm_elem_t int_ignored, ioopm_elem_t *cart_item, void *extra_ignored)
{
  ioopm_cart_item_t *cart_item_destroy = cart_item->cart_item_elem;
  cart_item_destroy->merch_name = NULL;
  cart_item_destroy->merch = NULL;
  // cart_item_destroy = NULL; // free(cart_item_destroy);
}

static void cart_destroy(ioopm_elem_t int_ignored, ioopm_elem_t *cart_to_destroy, void *extra_ignored) // AUX-funktion till cart_list_destroy.
{
  ioopm_cart_t *current_cart = cart_to_destroy->cart_elem;
  if (ioopm_hash_table_size(current_cart->products) > 0)
  {
    ioopm_hash_table_apply_to_all(current_cart->products, cart_item_nuke, NULL);
  }
  ioopm_hash_table_destroy(current_cart->products);
}

static void free_cart(ioopm_elem_t int_ignored, ioopm_elem_t *cart_to_destroy, void *extra_ignored) // AUX-funktion till cart_list_destroy.
{
  ioopm_cart_t *current_cart = cart_to_destroy->cart_elem;
  // free(current_cart);
}

void ioopm_cart_spec_destroy(ioopm_cart_list_t *cart_list, uint32_t cart_index)
{
  ioopm_link_t *current_link = cart_list->carts->first;
  ioopm_cart_t *cart_edit = current_link->element.cart_elem;
  int counter = 0;
  while (cart_edit && cart_edit->cart_id != cart_index)
  {
    current_link = current_link->next;
    cart_edit = current_link->element.cart_elem;
    counter++;
  }
  ioopm_elem_t *cart_products = ioopm_hash_table_values(cart_edit->products);
  ioopm_cart_item_t *current_item;
  for (int index = 0; index < ioopm_hash_table_size(cart_edit->products); index++)
  {
    current_item = cart_products[index].cart_item_elem;
    current_item->merch->stock_in_cart -= current_item->amount;
  }
  cart_destroy(Int_elem(0), &Cart_elem(cart_edit), NULL);
  // cart_edit = NULL; // free(cart_edit);
  ioopm_linked_list_remove(cart_list->carts, counter);
  cart_list->no_carts_removed += 1;
  // cart_edit = NULL; // free(cart_products);
}

void ioopm_cart_list_destroy(ioopm_cart_list_t *cart_list)
{
  if (cart_list->carts->size > 0)
  {
    ioopm_linked_list_apply_to_all(cart_list->carts, cart_destroy, NULL);
    ioopm_linked_list_apply_to_all(cart_list->carts, free_cart, NULL);
  }

  ioopm_linked_list_destroy(cart_list->carts);

  // cart_list = NULL; // free(cart_list);
}

static ioopm_cart_item_t *cart_item_create(char *merch_name, ioopm_merch_t *merch, uint32_t amount, heap_t *heap)
{
  ioopm_cart_item_t *cart_item = h_alloc_struct(heap, "**i"); // calloc(1, sizeof(ioopm_cart_item_t));
  cart_item->merch_name = merch->name;
  cart_item->merch = merch;
  cart_item->amount = amount;
  return cart_item;
}

static void cart_item_destroy(ioopm_cart_item_t *cart_item)
{
  cart_item->merch_name = NULL;
  cart_item->merch = NULL;
  // cart_item = NULL; // free(cart_item);
}

bool ioopm_cart_add_merch(ioopm_cart_list_t *cart_list, ioopm_hash_table_t *warehouse, ioopm_merch_t *merch, uint32_t cart_id, uint32_t amount)
{
  if (merch->total_stock >= (amount + (merch->stock_in_cart)))
  {
    ioopm_link_t *current_link = cart_list->carts->first;
    ioopm_cart_t *cart_edit = current_link->element.cart_elem;
    while (cart_edit && cart_edit->cart_id != cart_id)
    {
      current_link = current_link->next;
      cart_edit = current_link->element.cart_elem;
    }
    if (ioopm_hash_table_lookup(cart_edit->products, Merch_elem(merch)))
    {
      ioopm_cart_item_t *item_edit = ioopm_hash_table_lookup(cart_edit->products, Merch_elem(merch))->cart_item_elem;
      item_edit->amount += amount;
      item_edit->merch->stock_in_cart += amount;
      return true;
    }
    else
    {
      ioopm_cart_item_t *cart_item = cart_item_create(merch->name, merch, amount, cart_list->carts->heap);
      ioopm_hash_table_insert(cart_edit->products, Merch_elem(merch), Cart_item_elem(cart_item));
      (merch->stock_in_cart) += amount;
      return true;
    }
  }
  return false;
}

void ioopm_cart_remove_merch(ioopm_cart_list_t *cart_list, ioopm_hash_table_t *warehouse, ioopm_merch_t *merch, uint32_t cart_id, uint32_t amount_to_remove)
{
  ioopm_list_iterator_t *iter = ioopm_list_iterator(cart_list->carts);
  int index = 0;
  do
  {
    if (ioopm_iterator_current(iter).cart_elem->cart_id == cart_id)
    {
      ioopm_cart_t *cart_edit = ioopm_linked_list_get(cart_list->carts, index).value.cart_elem;

      ioopm_cart_item_t *cart_item_edit = ioopm_hash_table_lookup(cart_edit->products, Merch_elem(merch))->cart_item_elem;
      if (cart_item_edit->amount <= amount_to_remove)
      {
        merch->stock_in_cart -= cart_item_edit->amount;
        ioopm_hash_table_remove(cart_edit->products, Merch_elem(merch));
        cart_item_destroy(cart_item_edit);
      }
      else
      {
        printf("\n%s\n", cart_item_edit->merch->name);
        cart_item_edit->amount -= amount_to_remove;
        merch->stock_in_cart -= amount_to_remove;
      }
    }
    index++;
  } while (ioopm_iterator_next(iter).success); // side effect: changes iter to iter->next upon success
  ioopm_iterator_destroy(iter);
}

uint32_t ioopm_cart_total_cost(ioopm_hash_table_t *wh, ioopm_cart_list_t *cart_list, uint32_t cart_id)
{

  ioopm_cart_t *current_cart = ioopm_linked_list_get(cart_list->carts, cart_id).value.cart_elem;
  ioopm_list_t *all_merch = ioopm_hash_table_keys(current_cart->products);
  ioopm_elem_t *all_total_merch = ioopm_hash_table_values(current_cart->products);
  uint32_t total_price_merch = 0;
  for (int index = 0; index < all_merch->size; index++)
  {
    ioopm_cart_item_t *current_cart_item = all_total_merch[index].cart_item_elem;
    total_price_merch += (current_cart_item->amount) * (current_cart_item->merch->price);
  }
  ioopm_linked_list_destroy(all_merch);
  // all_total_merch = NULL; // free(all_total_merch);
  return total_price_merch;
}

void ioopm_cart_checkout(ioopm_hash_table_t *warehouse, ioopm_hash_table_t *shelves, ioopm_cart_list_t *cart_list, ioopm_cart_t *cart)
{
  ioopm_elem_t *elem_array = ioopm_hash_table_values(cart->products);

  for (int index = 0; index < ioopm_hash_table_size(cart->products); index++)
  {
    ioopm_cart_item_t *current_cart_item = elem_array[index].cart_item_elem;
    ioopm_list_iterator_t *iter = ioopm_list_iterator(current_cart_item->merch->locs);
    ioopm_shelf_t *current_shelf_in_merch = ioopm_iterator_current(iter).ptr_elem;
    while (current_shelf_in_merch && (current_cart_item->amount > 0))
    {
      if (current_shelf_in_merch->amount > current_cart_item->amount)
      {
        current_cart_item->merch->total_stock -= current_cart_item->amount;
        current_cart_item->merch->stock_in_cart -= current_cart_item->amount;

        current_shelf_in_merch->amount -= current_cart_item->amount;
        current_cart_item->amount = 0;
      }
      else if (current_shelf_in_merch->amount == current_cart_item->amount)
      {
        current_cart_item->merch->total_stock -= current_cart_item->amount;
        current_cart_item->merch->stock_in_cart -= current_cart_item->amount;

        shelf_spec_destroy(shelves, current_cart_item->merch, current_shelf_in_merch);
        current_cart_item->amount = 0;
      }
      else
      {
        current_cart_item->merch->total_stock -= current_shelf_in_merch->amount;
        current_cart_item->merch->stock_in_cart -= current_shelf_in_merch->amount;
        
        current_cart_item->amount -= current_shelf_in_merch->amount;
        ioopm_shelf_t *next = ioopm_iterator_next(iter).value.shelf_elem;
        shelf_spec_destroy(shelves, current_cart_item->merch, current_shelf_in_merch);
        current_shelf_in_merch = next;
      }
      
    }
    ioopm_iterator_destroy(iter);
  }

  ioopm_cart_spec_destroy(cart_list, cart->cart_id);
  // elem_array = NULL; // free(elem_array);
}
