#include "frontend.h"

/// @brief Prints name, description and price of a merch
/// @param merch pointer to the merch we want info about
static void print_merch(ioopm_merch_t *merch)
{
  printf("Name: %s\n", merch->name);
  printf("Desc: %s\n", merch->desc);
  printf("Price: %d SEK\n", merch->price);
}


/// @brief Asks user for input to make a merch and puts it in a warehouse
/// @param wh warehouse the merch will be created into
/// @param allocd_strs a linked list containing pointers to allocated strings
/// which are cleared later by the function free_frontend_allocs()
/// @return After user inputs a correctly formatted merch; 
/// A pointer to the newly created merch located in the warehouse wh.
/// If a user never enters valid input you will be stuck in an endless loop.
static ioopm_merch_t *input_merch(ioopm_hash_table_t *wh, ioopm_list_t *allocd_strs)
{
  char *name = "";
  bool occupied_merch = false;
  do
  {
    if (occupied_merch)
    {
      printf("This merch already exist!\n");
    }
    occupied_merch = true; // the error message will be shown when we loop.
    name = ask_question_string("Name: ", get_heap(wh));
    ioopm_linked_list_append(allocd_strs, Ptr_elem(name));
  } while (ioopm_hash_table_lookup(wh, Ptr_elem(name)) != NULL);
  char *desc = ask_question_string("Desc: ", get_heap(wh));
  ioopm_linked_list_append(allocd_strs, Ptr_elem(desc));
  int price = ask_question_positive_int("Price: ");
  ioopm_merch_t *merch = ioopm_merch_create(name, desc, price, get_heap(wh));
  return merch;
}


/// @brief Prints all values of a hash table with number lines starting from 1.
/// If there are more than 20 values in the hash table it will ask user for input
/// to confirm printing up to 20 more at a time until all have been printed.
/// @param wh hash table to be listed
/// @param message a string printed BEFORE the listing
/// @param allocd_strs a linked list containing pointers to allocated strings
/// which are cleared later by the function free_frontend_allocs()
/// @param allocd_elem_arrs a linked list containing pointers to allocated arrays
/// of elem_ts which are cleared later by the function free_frontend_allocs()
static void numbered_hash_table_print(ioopm_hash_table_t *wh, char *message, ioopm_list_t *allocd_strs, ioopm_list_t *allocd_elem_arrs)
{
  ioopm_elem_t *arr = ioopm_hash_table_values(wh);
  ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr));
  char *keep_showing = "";
  char *prompt =
      "Showing 20 listings at a time; type [m] to "
      "go to main menu, or [ANY] to list more\n";
  uint32_t merch_num = 0;
  uint32_t total_merches = ioopm_hash_table_size(wh);
  do
  {
    printf("%s", message);
    for (int merches_printed = 0; merches_printed < 20 && merch_num < total_merches; merch_num++, merches_printed++)
    {
      printf("%d. %s\n", merch_num + 1, arr[merch_num].merch_elem->name); // merch_num+1 för att människor räknar från 1.
    }
    if (merch_num == total_merches)
    {
      printf(" -- end of list -- \n");
    }
    else
    {
      keep_showing = ask_question_string(prompt, get_heap(wh));
      ioopm_linked_list_append(allocd_strs, Ptr_elem(keep_showing));
    }
  } while (strcmp(keep_showing, "M") && strcmp(keep_showing, "m") && merch_num != total_merches); // kommer ge andra värden än 0 tills nån av dem matchar
}


/// @brief Prints a menu with choices available for either a user or an admin.
/// @param admin bool indicating if user is an admin or not
static void print_menu(bool admin)
{
  char *admin_menu =
      "[L] Create new merch\n"
      "[R] Edit merch\n"
      "[F] Add stock for merch\n"
      "[K] Remove merch\n"
      "[V] List all merch in warehouse\n"
      "[H] Find locations for merch in warehouse\n"
      "[B] Switch user mode\n"
      "[A] Quit\n";
  char *user_menu =
      "[D] Create a new cart\n"
      "[E] Remove a cart\n"
      "[S] Put merch into a cart\n"
      "[T] Remove merch from a cart\n"
      "[V] List all merch in warehouse\n"
      "[P] Price check a cart\n"
      "[G] Make purchase with a cart\n"
      "[B] Switch user mode\n"
      "[A] Quit\n"; // Unused letters: CIJMNOQUWXYZ
  if (admin)
  {
    fputs(admin_menu, stdout);
  }
  else
  {
    fputs(user_menu, stdout);
  }
}

/// @brief Asks for input from user to get a char representing a menu choice
/// @param admin bool indicating if user is admin or not
/// @return uppercase char representing a choice from the menu made by the user
/// If a user never enters valid input you will be stuck in an endless loop.
static char ask_question_menu(bool admin)
{
  print_menu(admin);
  char c = '\0';
  bool error = false;
  do
  {
    if (error)
    {
      print("Error! please type a valid letter.\n");
    }
    print("Choice: ");
    c = toupper(getchar());
    clear_in_buffer();
    error = true;
  } while (!in(c, "ABDEFGHKLPRSTVX"));
  return c;
}

/// @brief apply_function used together with ioopm_linked_list_apply_to_all
/// to free all non-null pointers in a linked list with pointers as values
/// @param index_not_used param not used
/// @param element the element we want to free
/// @param extra_not_used param not used
static void free_non_null_ptr(ioopm_elem_t index_not_used, ioopm_elem_t *element, void *extra_not_used)
{
  if (element->ptr_elem)
  {
    free(element->ptr_elem);
    element->ptr_elem = NULL;
  }
}

/// @brief Function which frees all pointers inside two linked lists with pointer values.
/// @param allocd_strs a linked list containing pointers to allocated strings
/// @param allocd_elem_arrs a linked list containing pointers to allocated arrays
static void free_frontend_allocs(ioopm_list_t *allocd_strs, ioopm_list_t *allocd_elem_arrs)
{
  ioopm_linked_list_apply_to_all(allocd_strs, free_non_null_ptr, NULL);
  ioopm_linked_list_destroy(allocd_strs);
  ioopm_linked_list_apply_to_all(allocd_elem_arrs, free_non_null_ptr, NULL);
  ioopm_linked_list_destroy(allocd_elem_arrs);
}

/// @brief Calculates amount of available stock of a specific merch.
/// @param merch the merch we want to find stock for.
/// @param cart_list the list of all carts that could possibly contain the merch.
/// @return number of stock for a specific merch that are available (not in any other cart). 
static int available_stock(ioopm_merch_t *merch, ioopm_cart_list_t *cart_list)
{
  ioopm_link_t *curr_cart = cart_list->carts->first;
  int available = merch->total_stock;
  while (curr_cart)
  {
    if (ioopm_hash_table_lookup(curr_cart->element.cart_elem->products, Merch_elem(merch)))
    {
      available -= ioopm_hash_table_lookup(curr_cart->element.cart_elem->products, Merch_elem(merch))->cart_item_elem->amount;
    }
    curr_cart = curr_cart->next;
  }
  return available;
}

/// @brief Asks user to pick a number and returns the number as an index 
/// @param max total amount of available choices
/// @param prompt string printed to the user when asking for the number
/// @param error_message printed if user enters number less than 1 or more than 'max'
/// @return index for the choice picked by user, will always be in range [0..max-1].
/// If a user never enters valid input you will be stuck in an endless loop.
static int ask_for_index_up_to(int max, char *prompt, char *error_message)
{
  int num = 0;
  bool error = false;
  do
  {
    if (error)
    {
      printf("%s", error_message);
    }
    error = true;

    num = ask_question_positive_int(prompt) - 1;
  } while (num < 0 || max <= num);
  return num;
}

/// @brief Asks user to pick a number and returns the number 
/// @param max largest value the user is allowed to enter
/// @param prompt string printed to the user when asking for the number
/// @param error_message printed if user enters number less than 0 or more than 'max'
/// @return index for the choice picked by user, will always be in range [0..max].
/// If a user never enters valid input you will be stuck in an endless loop.
static int ask_for_amount_up_to(int max, char *prompt, char *error_message)
{
  int num = 0;
  bool error = false;
  do
  {
    if (error)
    {
      printf("%s", error_message);
    }
    error = true;

    num = ask_question_positive_int(prompt);
  } while (num < 0 || max < num);
  return num;
}

/// @brief "main function" which starts by printing the menu of options available
/// to the user and from there on the user will add and remove merches, shelf
/// locations, carts, etc and also simulate purchases, check prices, edit merch
/// and other various functionality we would expect from a simple web store.
/// @param heap the heap
static void event_loop(ioopm_hash_table_t *wh, ioopm_hash_table_t *shelves, ioopm_cart_list_t *cart_list, heap_t *heap)
{
  char c = '\0';
  bool admin = true;
  int merch_num = 0;
  int cart_item_num = 0;
  int warehouse_size = 0;
  int cart_num = 0;

  ioopm_list_t *allocd_strs = ioopm_linked_list_create(ioopm_int_eq, heap);
  ioopm_list_t *allocd_elem_arrs = ioopm_linked_list_create(ioopm_int_eq, heap);

  while (1)
  {
  event_loop_start:
    warehouse_size = ioopm_hash_table_size(wh);
    c = ask_question_menu(admin);

    if (c == 'A')
    {
      // ioopm_elem_t *arr = ioopm_hash_table_values(wh);
      // for (int i = 0; i < warehouse_size; i++)
      // {
      //   ioopm_merch_t *merch = (arr[i]).merch_elem;
      //   ioopm_product_destroy(shelves, wh, merch);
      // }
      // free(arr);
      // free_frontend_allocs(allocd_strs, allocd_elem_arrs);
      break;
    }
    else if (c == 'X') { // Manually trigger GC, hidden feature
      h_gc(heap);
      break;
    }
    else if (c == 'B')
    {
      admin = !admin;
      if (admin)
      {
        printf("Now in admin view.\n");
      }
      else
      {
        printf("Now in customer view.\n");
      }
    }
    else if (c == 'D')
    {
      ioopm_cart_create(cart_list);
      printf("Cart %d created! (Unique ID:%d)\n", cart_list->carts->size, cart_list->next_cart_id - 1);
    }
    else if (c == 'E')
    {
      if (cart_list->carts->size > 0)
      {
        int selected_cart = 0;
        ioopm_link_t *p_cart = cart_list->carts->first;
        printf("Listing all carts:\n");
        for (int i = 0; i < cart_list->carts->size; i++)
        {
          printf("Cart %d (Unique ID:%d)\n", i + 1, p_cart->element.cart_elem->cart_id);
          p_cart = p_cart->next;
        }

        selected_cart = ask_for_index_up_to(cart_list->carts->size, "Which cart do you want to remove?\nCart Number: ", "Error: Cart does not exist!\n");

        ioopm_cart_t *our_cart = ioopm_linked_list_get(cart_list->carts, selected_cart).value.cart_elem;
        ioopm_cart_spec_destroy(cart_list, our_cart->cart_id);
        printf("Successfully removed the cart\n");
      }
      else
      {
        printf("Error: There are no carts!\n");
      }
    }
    else if (c == 'F' && admin && warehouse_size)
    {
      numbered_hash_table_print(wh, "Listing warehouse:\n", allocd_strs, allocd_elem_arrs);

      merch_num = ask_for_index_up_to(warehouse_size, "Which merch do you want to replenish?\nMerch number: ", "Error: Merch number out of range!\n");

      ioopm_elem_t *arr = ioopm_hash_table_values(wh);
      ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr));
      ioopm_merch_t *merch = arr[merch_num].merch_elem;

      uint32_t amount = ask_question_positive_int("How much stock is added?\nAmount: ");
      char *shelf_name = "";
      bool occupied_shelf = false;

      do
      {
        if (occupied_shelf)
        {
          printf("Error: Shelf already in use!\n");
        }
        occupied_shelf = true;

        shelf_name = ask_question_shelf("Where is this new stock located?\nShelf: ", heap);
        ioopm_linked_list_append(allocd_strs, Ptr_elem(shelf_name));
      } while (!ioopm_is_shelf_free(shelves, shelf_name, merch));

      ioopm_merch_replenish(shelves, merch, amount, shelf_name);
    }
    else if (c == 'G')
    {
      if (ioopm_merch_in_cart_any(cart_list))
      {
        printf("Listing all carts:\n");
        for (int i = 0; i < cart_list->carts->size; i++)
        {
          printf("Cart %d\n", i + 1);
        }

        cart_num = ask_for_index_up_to(cart_list->carts->size, "Which cart do you want to check out?\nCart number: ", "Error: Cart does not exist!\n");

        printf("Total cost of cart: %d\n", ioopm_cart_total_cost(wh, cart_list, cart_num));
        ioopm_cart_t *curr_cart;
        curr_cart = ioopm_linked_list_get(cart_list->carts, cart_num).value.cart_elem; // HÄR HAR VI EN ANNAN LÖSNING (SOM INTE ANVÄNDER ARRAY) ÄN ALLA ANDRA
        ioopm_cart_checkout(wh, shelves, cart_list, curr_cart);
        printf("Successfully checked out cart\n");
      }
      else
      {
        printf("Error: No merch in cart(s)!\n");
      }
    }
    else if (c == 'H' && admin && warehouse_size)
    {
      numbered_hash_table_print(wh, "Listing warehouse:\n", allocd_strs, allocd_elem_arrs);

      merch_num = ask_for_index_up_to(warehouse_size, "Which merch do you want to find?\nMerch number: ", "Error: Merch number out of range!\n");

      ioopm_elem_t *arr = ioopm_hash_table_values(wh);
      ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr));
      ioopm_merch_t *merch = arr[merch_num].merch_elem;

      ioopm_list_t *shelf_list = merch->locs;
      if (shelf_list->size)
      {
        for (int i = 0; i < shelf_list->size; i++)
        {
          ioopm_shelf_t *current_shelf = ioopm_linked_list_get(shelf_list, i).value.shelf_elem;
          printf("There are currently %d %s at %s\n", current_shelf->amount, merch->name, current_shelf->shelf_name);
        }
      }
      else
      {
        printf("Error: The stock for that merch is empty!\n");
      }
    }
    else if (c == 'K' && admin && warehouse_size)
    {
      numbered_hash_table_print(wh, "Listing warehouse:\n", allocd_strs, allocd_elem_arrs);
      merch_num = ask_for_index_up_to(warehouse_size, "Which merch do you want to remove?\nMerch number: ", "Error: Merch number out of range!\n");

      ioopm_elem_t *arr = ioopm_hash_table_values(wh);
      ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr));
      ioopm_merch_t *merch = arr[merch_num].merch_elem;

      if (!(merch->stock_in_cart > 0))
      {
        ioopm_product_destroy(shelves, wh, merch);
        printf("Successfully removed the merch\n");
      }
      else
      {
        printf("Error: Deleting a merch that is currently placed in a cart is not allowed!\n");
      }
    }
    else if (c == 'L' && admin)
    {
      ioopm_merch_t *new_merch = input_merch(wh, allocd_strs);
      ioopm_merch_insert(wh, new_merch);
    }
    else if (c == 'P' && warehouse_size)
    {
      if (cart_list->carts->size > 0)
      {
        int selected_cart = 0;
        ioopm_link_t *p_cart = cart_list->carts->first;

        printf("Listing all carts:\n");
        for (int i = 0; i < cart_list->carts->size; i++)
        {
          printf("Cart %d (Unique ID:%d)\n", i + 1, p_cart->element.cart_elem->cart_id);
          p_cart = p_cart->next;
        }

        selected_cart = ask_for_index_up_to(cart_list->carts->size, "Which cart do you want to price check?\nCart Number: ", "Error: Cart does not exist!\n");

        printf("Price: %d\n", ioopm_cart_total_cost(wh, cart_list, selected_cart));
      }
      else
      {
        printf("Error: There are no carts!\n");
      }
    }
    else if (c == 'R' && admin && warehouse_size)
    {
      numbered_hash_table_print(wh, "Listing warehouse:\n", allocd_strs, allocd_elem_arrs);

      merch_num = ask_for_index_up_to(warehouse_size, "Which merch do you want to edit?\nMerch number: ", "Error: Merch number out of range!\n");

      ioopm_elem_t *arr = ioopm_hash_table_values(wh);
      ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr));
      ioopm_merch_t *merch = arr[merch_num].merch_elem;

      if (merch->stock_in_cart > 0)
      {
        printf("Error: Editing a merch while it's in a cart is not allowed!\n");
        goto event_loop_start;
      }
      print_merch(merch);

      char flags = ask_for_amount_up_to(7, "1: Edit name\n2: Edit price\n3: Edit name & price\n4: Edit description\
        \n5: Edit name & description\n6: Edit price & description\n7: Edit all\n0: Cancel\nPick option from above(0-7): ",\
        "Error: Option out of range\n");

      if (flags)
      {
        printf("Editing %s, please update values.\n", merch->name);
        char *new_name = "";
        char *new_desc = "";
        int new_price = 0;
        if (flags & EDIT_NAME)
        {
          bool occupied_merch = false;
          do
          {
            if (occupied_merch)
            {
              printf("Error: This merch already exists! Use another name!\n");
            }
            occupied_merch = true; // the error message will be shown when we loop.
            new_name = ask_question_string("New name: ", heap);
            ioopm_linked_list_append(allocd_strs, Ptr_elem(new_name));
          } while (ioopm_hash_table_lookup(wh, Ptr_elem(new_name)) != NULL);
        }
        if (flags & EDIT_DESC)
        {
          new_desc = ask_question_string("New description: ", heap);
          ioopm_linked_list_append(allocd_strs, Ptr_elem(new_desc));
        }
        if (flags & EDIT_PRICE)
        {
          new_price = ask_question_positive_int("New price: ");
        }
        ioopm_merch_edit(wh, merch, new_name, new_desc, new_price, flags);
      }
      else
      {
        printf("Edit canceled.\n");
      }
    }
    else if (c == 'S' && warehouse_size)
    {
      if (cart_list->carts->size > 0)
      {
        ioopm_elem_t *arr = calloc(cart_list->carts->size, sizeof(ioopm_elem_t *));
        ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr));

        ioopm_elem_t curr_cart;
        printf("Listing all carts:\n");
        for (int i = 0; i < cart_list->carts->size; i++)
        {
          curr_cart = ioopm_linked_list_get(cart_list->carts, i).value;
          printf("Cart %d(Unique ID:%d)\n", i + 1, curr_cart.cart_elem->cart_id);
          arr[i] = curr_cart;
        }

        int selected_cart = ask_for_index_up_to(cart_list->carts->size, "Which cart do you want to add merch into?\nCart Number: ", "Error: Cart does not exist!\n");

        ioopm_cart_t *our_cart = arr[selected_cart].cart_elem;

        numbered_hash_table_print(wh, "Listing warehouse:\n", allocd_strs, allocd_elem_arrs);
        merch_num = ask_for_index_up_to(warehouse_size, "Which merch do you want to add to cart?\nMerch number: ", "Error: Merch number out of range!\n");

        ioopm_elem_t *arr2 = ioopm_hash_table_values(wh);
        ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr2));
        ioopm_merch_t *merch = arr2[merch_num].merch_elem;

        int curr_available = available_stock(merch, cart_list);

        if (curr_available)
        {
          printf("There are currently %d available %s in stock.\n", curr_available, merch->name);

          int amount = ask_for_amount_up_to(curr_available,"How many do you want to add to cart?\nAmount: ","Error: Invalid amount!\n");

          ioopm_cart_add_merch(cart_list, wh, merch, our_cart->cart_id, amount);
          printf("Added %d %s into cart %d(unique id:%d)\n", amount, merch->name, selected_cart, our_cart->cart_id);
        }
        else
        {
          printf("Error: There are 0 %s left in stock\n", merch->name);
        }
        free(arr);
      }
      else if (!ioopm_merch_is_replenished(wh))
      {
        printf("Error: There are no merch in stock!\n");
      }
      else
      {
        printf("Error: There are no carts!\n");
      }
    }
    else if (c == 'T')
    {
      if (ioopm_merch_in_cart_any(cart_list))
      {
        int selected_cart = 0;

        ioopm_elem_t *arr = calloc(cart_list->carts->size, sizeof(ioopm_elem_t *));
        ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr));
        ioopm_elem_t curr_cart;
        printf("Listing all carts:\n");

        for (int i = 0; i < cart_list->carts->size; i++) // XD
        {
          curr_cart = ioopm_linked_list_get(cart_list->carts, i).value;
          printf("Cart %d(Unique ID:%d)\n", i + 1, curr_cart.cart_elem->cart_id);
          arr[i] = curr_cart;
        }

        ioopm_cart_t *our_cart = arr[selected_cart].cart_elem;

        bool valid_cart = true;
        do
        {
          if (!valid_cart)
          {
            printf("Error: Cart is empty!\n");
          }
          valid_cart = false;

          selected_cart = ask_for_index_up_to(cart_list->carts->size, "Which cart do you want to remove merch from?\nCart number: ", "Error: Cart does not exist!\n");

          our_cart = arr[selected_cart].cart_elem;

        } while (ioopm_hash_table_size(our_cart->products) < 1);

        numbered_hash_table_print(our_cart->products, "Listing all products in cart:\n", allocd_strs, allocd_elem_arrs);
        int cart_size = ioopm_hash_table_size(our_cart->products);
        cart_item_num = ask_for_index_up_to(cart_size, "Which merch do you want to remove from cart?\nMerch number: ", "Error: Merch number out of range!\n");

        ioopm_elem_t *arr2 = ioopm_hash_table_values(our_cart->products);
        ioopm_linked_list_append(allocd_elem_arrs, Ptr_elem(arr2));
        ioopm_cart_item_t *cart_item = arr2[cart_item_num].cart_item_elem;

        printf("There are currently %d %s in cart %d (Unique ID:%d)\n", cart_item->amount, cart_item->merch->name, selected_cart, our_cart->cart_id);

        int amount = ask_for_amount_up_to(cart_item->amount,"How many do you want to remove?\nAmount: ","Error: Amount out of range!\n");

        char *name = strdup(cart_item->merch->name);
        printf("Removed %d %s from cart %d (Unique ID:%d)\n", amount, name, selected_cart, our_cart->cart_id);
        ioopm_cart_remove_merch(cart_list, wh, cart_item->merch, our_cart->cart_id, amount);
        free(name);
        free(arr);
      }
      else if (warehouse_size < 1)
      {
        printf("Error: The warehouse is empty!\n");
      }
      else if (!ioopm_merch_is_replenished(wh))
      {
        printf("Error: There are no merch in stock!\n");
      }
      else if (!ioopm_merch_in_cart_any(cart_list))
      {
        printf("Error: There aren't any merch in the cart(s)!\n");
      }
      else
      {
        printf("Error: There are no carts!\n");
      }
    }
    else if (c == 'V' && warehouse_size)
    {
      numbered_hash_table_print(wh, "Listing all products in warehouse:\n", allocd_strs, allocd_elem_arrs);
    }
    else
    {
      if (warehouse_size == 0)
      {
        printf("Error: Warehouse is empty!\n");
      }
      else if (!admin)
      {
        printf("Error: You are not admin!\n");
      }
      else
      {
        printf("Error: Unknown error!\n");
      }
    }
  }
}

#define HEAP_SIZE P_ALLOC_SIZE * 8 + h_metadata_size() + sizeof(bitvector_t)

/// @brief runs the event loop after initiating data structures we need for it
/// and also tears them down after we are done with the event loop.
int main(void)
{
  heap_t *heap = h_init(HEAP_SIZE, true, 0.5); // FIXME: is 6kB enough? is stack unsafe? what is a good threshold

  ioopm_hash_table_t *wh = ioopm_warehouse_create(heap);
  ioopm_hash_table_t *shelves = ioopm_shelves_create(heap);
  ioopm_cart_list_t *cart_list = ioopm_cart_list_create(heap);
  
  event_loop(wh, shelves, cart_list, heap);

  h_delete(heap);

  // Should not be needed
  // ioopm_cart_list_destroy(cart_list);
  // ioopm_shelves_destroy(shelves);
  // ioopm_warehouse_destroy(wh);
  return 0;
}