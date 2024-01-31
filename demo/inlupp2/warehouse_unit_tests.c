#include <CUnit/Basic.h>
#include "common.h"
#include "linked_list.h"
#include "warehouse.h"
#include "shelves.h"

int init_suite(void)
{
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void)
{
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

void test_create_destroy_warehouse()
{
  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  CU_ASSERT_PTR_NOT_NULL(wh);
  ioopm_warehouse_destroy(wh);
}

void test_merch_create()
{
  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name = "Apple";
  char *desc = "A fruit";
  int price = 5;
  ioopm_merch_t *new_merch = ioopm_merch_create(name, desc, price);

  CU_ASSERT_EQUAL(new_merch->name, "Apple");
  CU_ASSERT_EQUAL(new_merch->desc, "A fruit");
  CU_ASSERT_EQUAL(new_merch->price, 5);

  ioopm_product_destroy(NULL, wh, new_merch);
  ioopm_warehouse_destroy(wh);
}

void test_warehouse_insert()
{
  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;

  char *name2 = "Pen";
  char *desc2 = "School utencil";
  int price2 = 10;

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_t *new_merch2 = ioopm_merch_create(name2, desc2, price2);

  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_insert(wh, new_merch2);

  CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(wh, Ptr_elem(new_merch1->name)));
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(wh, Ptr_elem(new_merch1->name))->merch_elem, new_merch1);

  CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(wh, Ptr_elem(new_merch2->name)));
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(wh, Ptr_elem(new_merch2->name))->merch_elem, new_merch2);

  ioopm_product_destroy(NULL, wh, new_merch1);
  ioopm_product_destroy(NULL, wh, new_merch2);
  ioopm_warehouse_destroy(wh);
}

void test_merch_replenish()
{
  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  ioopm_hash_table_t *shelves = ioopm_shelves_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;
  char *shelf1 = "A01";

  char *name2 = "Pen";
  char *desc2 = "School utencil";
  int price2 = 10;
  char *shelf2 = "X81";
  char *shelf3 = "Ä71";

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_t *new_merch2 = ioopm_merch_create(name2, desc2, price2);

  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_insert(wh, new_merch2);
  CU_ASSERT_FALSE(ioopm_merch_is_replenished(wh))
  ioopm_merch_replenish(shelves, new_merch1, 83, shelf2);
  ioopm_merch_replenish(shelves, new_merch1, 30, shelf1);
  ioopm_merch_replenish(shelves, new_merch1, 30, shelf1);
  ioopm_merch_replenish(shelves, new_merch1, 98, shelf3);
  CU_ASSERT_TRUE(ioopm_merch_is_replenished(wh));
  CU_ASSERT_EQUAL(new_merch1->total_stock, 241);

  CU_ASSERT_EQUAL(ioopm_linked_list_get(new_merch1->locs, 0).value.shelf_elem->amount, 98);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(new_merch1->locs, 1).value.shelf_elem->amount, 60);
  CU_ASSERT_EQUAL(ioopm_linked_list_get(new_merch1->locs, 2).value.shelf_elem->amount, 83);

  ioopm_product_destroy(NULL, wh, new_merch1);
  ioopm_product_destroy(NULL, wh, new_merch2);

  ioopm_shelves_destroy(shelves);
  ioopm_warehouse_destroy(wh);
}

void test_merch_name_edit()
{
  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;

  char *name2 = "Pie";
  char *desc2 = "A dessert";
  int price2 = 15;

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_t *new_merch2 = ioopm_merch_create(name2, desc2, price2);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_insert(wh, new_merch2);

  CU_ASSERT_EQUAL(new_merch1->name, "Apple");
  CU_ASSERT_EQUAL(new_merch1->price, 5);
  ioopm_merch_edit_price(wh, new_merch1, 10);
  ioopm_merch_edit_name(wh, new_merch1, "Glock 18");
  ioopm_merch_edit_desc(wh, new_merch1, "A gun");

  CU_ASSERT_EQUAL(new_merch1->price, 10);
  CU_ASSERT_EQUAL(new_merch1->name, "Glock 18");
  CU_ASSERT_EQUAL(new_merch1->desc, "A gun");

  CU_ASSERT_EQUAL(new_merch2->name, "Pie");
  CU_ASSERT_EQUAL(new_merch2->price, 15);
  CU_ASSERT_EQUAL(new_merch2->desc, "A dessert");

  ioopm_elem_t *merch_arr = ioopm_hash_table_values(wh);
  CU_ASSERT_EQUAL(ioopm_hash_table_size(wh), 2);
  ioopm_product_destroy(NULL, wh, new_merch1);
  ioopm_product_destroy(NULL, wh, new_merch2);
  ioopm_warehouse_destroy(wh);
  free(merch_arr);
}

void test_cart_create_destroy()
{
  ioopm_cart_list_t *test_cart_list = ioopm_cart_list_create();
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  CU_ASSERT_PTR_NOT_NULL(test_cart_list);
  ioopm_cart_list_destroy(test_cart_list);
}

void test_cart_spec_destroy()
{
  ioopm_cart_list_t *test_cart_list = ioopm_cart_list_create();
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);

  CU_ASSERT_PTR_NOT_NULL(test_cart_list);
  ioopm_cart_spec_destroy(test_cart_list, 1); // Removing the middle element in linked list
  CU_ASSERT_EQUAL(test_cart_list->next_cart_id, 3);
  CU_ASSERT_EQUAL(test_cart_list->carts->first->element.cart_elem->cart_id, 0);
  CU_ASSERT_EQUAL(test_cart_list->carts->last->element.cart_elem->cart_id, 2);
  ioopm_cart_list_destroy(test_cart_list);
}

void test_cart_add_merch()
{
  ioopm_cart_list_t *test_cart_list = ioopm_cart_list_create();
  ioopm_hash_table_t *shelves = ioopm_shelves_create();
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);

  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;
  char *shelf1 = "A70";

  char *name2 = "Hammer";
  char *desc2 = "A tool";
  int price2 = 170;
  char *shelf2 = "Z30";

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_t *new_merch2 = ioopm_merch_create(name2, desc2, price2);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_replenish(shelves, new_merch1, 30, shelf1);
  ioopm_merch_replenish(shelves, new_merch2, 10, shelf2);

  ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 0, 3);
  ioopm_cart_add_merch(test_cart_list, wh, new_merch2, 0, 4);
  ioopm_cart_add_merch(test_cart_list, wh, new_merch2, 1, 5);
  ioopm_cart_add_merch(test_cart_list, wh, new_merch2, 2, 6); // Denna kommer aldrig inserta

  CU_ASSERT_EQUAL(new_merch1->stock_in_cart, 3);
  CU_ASSERT_EQUAL(new_merch2->stock_in_cart, 9);

  ioopm_cart_t *current_cart1 = ioopm_linked_list_get(test_cart_list->carts, 0).value.cart_elem;
  ioopm_cart_t *current_cart2 = ioopm_linked_list_get(test_cart_list->carts, 1).value.cart_elem; // Hämtar ut cartsen ur länkade listan, SHIT PERFORMANCE!
  ioopm_cart_t *current_cart3 = ioopm_linked_list_get(test_cart_list->carts, 2).value.cart_elem;

  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(current_cart1->products, Merch_elem(new_merch1))->cart_item_elem->amount, 3);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(current_cart1->products, Merch_elem(new_merch2))->cart_item_elem->amount, 4);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(current_cart2->products, Merch_elem(new_merch2))->cart_item_elem->amount, 5);
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(current_cart3->products, Merch_elem(new_merch2))); //

  ioopm_product_destroy(NULL, wh, new_merch1);
  ioopm_product_destroy(NULL, wh, new_merch2);
  ioopm_warehouse_destroy(wh);
  ioopm_cart_list_destroy(test_cart_list);
  ioopm_shelves_destroy(shelves);
}

void test_cart_remove_merch()
{
  ioopm_cart_list_t *test_cart_list = ioopm_cart_list_create();
  ioopm_hash_table_t *shelves = ioopm_shelves_create();
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);

  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;
  char *shelf1 = "A70";

  char *name2 = "Hammer";
  char *desc2 = "A tool";
  int price2 = 170;
  char *shelf2 = "Z30";

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_t *new_merch2 = ioopm_merch_create(name2, desc2, price2);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_replenish(shelves, new_merch1, 30, shelf1);
  ioopm_merch_replenish(shelves, new_merch2, 10, shelf2);

  ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 0, 3);
  ioopm_cart_t *current_cart1 = ioopm_linked_list_get(test_cart_list->carts, 0).value.cart_elem;

  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(current_cart1->products, Merch_elem(new_merch1))->cart_item_elem->amount, 3);

  ioopm_cart_remove_merch(test_cart_list, wh, new_merch1, 0, 1);

  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(current_cart1->products, Merch_elem(new_merch1))->cart_item_elem->amount, 2);
  ioopm_product_destroy(shelves, wh, new_merch1);
  ioopm_product_destroy(shelves, wh, new_merch2);
  ioopm_warehouse_destroy(wh);
  ioopm_cart_list_destroy(test_cart_list);
  ioopm_shelves_destroy(shelves);
}

void test_cart_total_cost()
{
  ioopm_cart_list_t *test_cart_list = ioopm_cart_list_create();
  ioopm_hash_table_t *shelves = ioopm_shelves_create();
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);

  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;
  char *shelf1 = "A70";

  char *name2 = "Hammer";
  char *desc2 = "A tool";
  int price2 = 170;
  char *shelf2 = "Z30";

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_t *new_merch2 = ioopm_merch_create(name2, desc2, price2);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_replenish(shelves, new_merch1, 30, shelf1);
  ioopm_merch_replenish(shelves, new_merch2, 10, shelf2);

  ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 0, 3);
  ioopm_cart_t *current_cart1 = ioopm_linked_list_get(test_cart_list->carts, 0).value.cart_elem;

  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(current_cart1->products, Merch_elem(new_merch1))->cart_item_elem->amount, 3);

  ioopm_cart_remove_merch(test_cart_list, wh, new_merch1, 0, 1);

  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(current_cart1->products, Merch_elem(new_merch1))->cart_item_elem->amount, 2);
  CU_ASSERT_EQUAL(ioopm_cart_total_cost(wh, test_cart_list, 0), 10);
  ioopm_product_destroy(shelves, wh, new_merch1);
  ioopm_product_destroy(shelves, wh, new_merch2);
  ioopm_warehouse_destroy(wh);
  ioopm_cart_list_destroy(test_cart_list);
  ioopm_shelves_destroy(shelves);
}

void test_cart_checkout()
{

  ioopm_cart_list_t *test_cart_list = ioopm_cart_list_create();
  ioopm_hash_table_t *shelves = ioopm_shelves_create();
  ioopm_cart_create(test_cart_list);
  ioopm_cart_t *test_cart = test_cart_list->carts->first->element.cart_elem;

  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;
  char *shelf1 = "A70";
  char *shelf2 = "B30";

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_replenish(shelves, new_merch1, 10, shelf1);
  ioopm_merch_replenish(shelves, new_merch1, 30, shelf2);
  ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 0, 5);

  ioopm_cart_checkout(wh, shelves, test_cart_list, test_cart);
  CU_ASSERT_EQUAL(new_merch1->total_stock, 35);
  printf("\n%d\n", new_merch1->stock_in_cart);
  CU_ASSERT_EQUAL(new_merch1->stock_in_cart, 0);

  ioopm_cart_create(test_cart_list);
  ioopm_cart_t *test_cart2 = test_cart_list->carts->first->element.cart_elem;

  ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 1, 30); // OBS CART ID KOMMER INTE STÄMMA ÖVERENS EFTER VI CHECKAR UT EN CART!!!
  ioopm_cart_checkout(wh, shelves, test_cart_list, test_cart2);
  CU_ASSERT_EQUAL(new_merch1->total_stock, 5);
  CU_ASSERT_EQUAL(new_merch1->locs->first->element.shelf_elem->amount, 5);

  ioopm_product_destroy(shelves, wh, new_merch1);
  ioopm_warehouse_destroy(wh);
  ioopm_cart_list_destroy(test_cart_list);
  ioopm_shelves_destroy(shelves);
}

void test_multiple_merch_checkout()
{
  ioopm_cart_list_t *test_cart_list = ioopm_cart_list_create();
  ioopm_hash_table_t *shelves = ioopm_shelves_create();
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_create(test_cart_list);
  ioopm_cart_t *test_cart1 = ioopm_linked_list_get(test_cart_list->carts, 0).value.cart_elem;
  ioopm_cart_t *test_cart2 = ioopm_linked_list_get(test_cart_list->carts, 1).value.cart_elem;
  ioopm_cart_t *test_cart3 = ioopm_linked_list_get(test_cart_list->carts, 2).value.cart_elem;
  CU_ASSERT_FALSE(ioopm_merch_in_cart_any(test_cart_list));

  ioopm_hash_table_t *wh = ioopm_warehouse_create();
  char *name1 = "Apple";
  char *desc1 = "A fruit";
  int price1 = 5;
  char *shelf1 = "A70";
  char *shelf2 = "B30";

  char *name2 = "Hammer";
  char *desc2 = "A tool";
  int price2 = 170;
  char *shelf3 = "Z30";
  char *shelf4 = "X42";

  char *name3 = "Tire";
  char *desc3 = "Car part";
  int price3 = 17000;
  char *shelf5 = "L23";

  ioopm_merch_t *new_merch1 = ioopm_merch_create(name1, desc1, price1);
  ioopm_merch_t *new_merch2 = ioopm_merch_create(name2, desc2, price2);
  ioopm_merch_t *new_merch3 = ioopm_merch_create(name3, desc3, price3);

  ioopm_merch_insert(wh, new_merch1);
  ioopm_merch_insert(wh, new_merch2);
  ioopm_merch_insert(wh, new_merch3);

  ioopm_merch_replenish(shelves, new_merch1, 10, shelf1); // MERCH 1
  ioopm_merch_replenish(shelves, new_merch1, 5, shelf2);

  ioopm_merch_replenish(shelves, new_merch2, 1, shelf3); // MERCH 2
  ioopm_merch_replenish(shelves, new_merch2, 4, shelf4);

  ioopm_merch_replenish(shelves, new_merch3, 2, shelf5); // MERCH 3

  ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 1, 11);
  ioopm_cart_add_merch(test_cart_list, wh, new_merch2, 1, 2);                  // CART ADD I CART2
  CU_ASSERT_FALSE(ioopm_cart_add_merch(test_cart_list, wh, new_merch3, 1, 3)); // Ska faila
  CU_ASSERT_EQUAL(new_merch3->stock_in_cart, 0);
  CU_ASSERT_EQUAL(new_merch1->stock_in_cart, 11);
  CU_ASSERT_EQUAL(new_merch2->stock_in_cart, 2);
  CU_ASSERT_TRUE(ioopm_merch_in_cart_any(test_cart_list));

  ioopm_cart_checkout(wh, shelves, test_cart_list, test_cart2); // CHECKOUT CART2
  printf("\n%d\n", new_merch1->stock_in_cart);
  CU_ASSERT_EQUAL(new_merch1->total_stock, 4);
  CU_ASSERT_EQUAL(new_merch2->total_stock, 3);
  CU_ASSERT_EQUAL(new_merch3->total_stock, 2);
  CU_ASSERT_EQUAL(new_merch1->stock_in_cart, 0);
  CU_ASSERT_EQUAL(new_merch2->stock_in_cart, 0);
  CU_ASSERT_EQUAL(new_merch3->stock_in_cart, 0);

  ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 0, 4);
  ioopm_cart_add_merch(test_cart_list, wh, new_merch2, 0, 3);
  ioopm_cart_add_merch(test_cart_list, wh, new_merch3, 0, 2);

  CU_ASSERT_EQUAL(ioopm_cart_total_cost(wh, test_cart_list, 0), 34530);
  ioopm_cart_checkout(wh, shelves, test_cart_list, test_cart1);

  CU_ASSERT_EQUAL(new_merch1->total_stock, 0);
  CU_ASSERT_EQUAL(new_merch2->total_stock, 0);
  CU_ASSERT_EQUAL(new_merch3->total_stock, 0);

  CU_ASSERT_FALSE(ioopm_cart_add_merch(test_cart_list, wh, new_merch1, 2, 4));
  CU_ASSERT_FALSE(ioopm_cart_add_merch(test_cart_list, wh, new_merch2, 2, 4));
  CU_ASSERT_FALSE(ioopm_cart_add_merch(test_cart_list, wh, new_merch3, 2, 4));
  CU_ASSERT_EQUAL(ioopm_hash_table_size(test_cart3->products), 0);

  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(shelves, Ptr_elem(shelf1)));
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(shelves, Ptr_elem(shelf2)));
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(shelves, Ptr_elem(shelf3)));
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(shelves, Ptr_elem(shelf4)));
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(shelves, Ptr_elem(shelf5)));

  CU_ASSERT_EQUAL(ioopm_hash_table_size(shelves), 0);

  ioopm_product_destroy(shelves, wh, new_merch1);
  ioopm_product_destroy(shelves, wh, new_merch2);
  ioopm_product_destroy(shelves, wh, new_merch3);
  ioopm_warehouse_destroy(wh);
  ioopm_cart_list_destroy(test_cart_list);
  ioopm_shelves_destroy(shelves);
}

int main()
{
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite warehouse_tests = CU_add_suite("Testing warehouse functionality", init_suite, clean_suite);
  if (warehouse_tests == NULL)
  {
    // If the test suite could not be added, tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
  if (
      (CU_add_test(warehouse_tests, "Creating and destroying a warehouse", test_create_destroy_warehouse) == NULL) ||
      (CU_add_test(warehouse_tests, "Creating new merch", test_merch_create) == NULL) ||
      (CU_add_test(warehouse_tests, "Inserting a merchandise into the warehouse", test_warehouse_insert) == NULL) ||
      (CU_add_test(warehouse_tests, "Testing merch count", test_merch_replenish) == NULL) ||
      (CU_add_test(warehouse_tests, "Testing editing name on merch", test_merch_name_edit) == NULL) ||
      (CU_add_test(warehouse_tests, "Creating and destroying a shopping cart", test_cart_create_destroy) == NULL) ||
      (CU_add_test(warehouse_tests, "Testing destroying specific cart in cart list", test_cart_spec_destroy) == NULL) ||
      (CU_add_test(warehouse_tests, "Test adding merch to a cart", test_cart_add_merch) == NULL) ||
      (CU_add_test(warehouse_tests, "Test removing merch from a cart", test_cart_remove_merch) == NULL) ||
      (CU_add_test(warehouse_tests, "Test calculating total cost of a specific cart", test_cart_total_cost) == NULL) ||
      (CU_add_test(warehouse_tests, "Test trying to checkout a single cart", test_cart_checkout) == NULL) ||
      (CU_add_test(warehouse_tests, "Test trying to checkout multiple carts and items", test_multiple_merch_checkout) == NULL) ||

      0)
  {
    // If adding any of the tests fails, we tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
  // Use CU_BRM_NORMAL to only print errors and a summary
  CU_basic_set_mode(CU_BRM_VERBOSE);

  // This is where the tests are actually run!
  CU_basic_run_tests();

  // Tear down CUnit before exiting
  CU_cleanup_registry();
  return CU_get_error();
}
