#include <CUnit/Basic.h>
#include "linked_list.h"
#include <stdbool.h>

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

void change_all_elements(ioopm_elem_t index, ioopm_elem_t *element, void *x)
  {
    ioopm_elem_t *new_element_ptr = x;
    ioopm_elem_t new_element = *new_element_ptr;

    *element = new_element;
  }

void change_to_16(ioopm_list_t *list)
{
  int new_element = 16;

  ioopm_linked_list_apply_to_all(list, change_all_elements, &new_element);

}

/// @brief Creating and destroying linked list
void test_create_destroy_ll() 
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  ioopm_linked_list_destroy(ll);
}

/// @brief Appending to linked list
void test_append_ll()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  int values[] = {3, -4, 0};

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
    
    ioopm_option_t result = ioopm_linked_list_get(ll, i);

    CU_ASSERT_TRUE(Successful(result));
    CU_ASSERT_EQUAL(result.value.int_elem, values[i]);
  }
  ioopm_linked_list_destroy(ll);
}

/// @brief Prepending to linked list
void test_prepend_ll()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  int values[] = {3, -4, 0};

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_prepend(ll, Int_elem(values[i]));

    ioopm_option_t result = ioopm_linked_list_get(ll, 0);

    CU_ASSERT_TRUE(Successful(result));
    CU_ASSERT_EQUAL(result.value.int_elem, values[i]);
  }
  
  ioopm_linked_list_destroy(ll);
}

/// @brief Appending and prepending to linked list
void test_append_and_prepend()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  int values[] = {3, -4, 0};

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]+1));
    ioopm_linked_list_prepend(ll, Int_elem(values[i]-1));

    ioopm_option_t result = ioopm_linked_list_get(ll, 0);
    CU_ASSERT_TRUE(Successful(result));
    CU_ASSERT_EQUAL(result.value.int_elem, values[i]-1);

    result = ioopm_linked_list_get(ll, ioopm_linked_list_size(ll)-1);
    CU_ASSERT_TRUE(Successful(result));
    CU_ASSERT_EQUAL(result.value.int_elem, values[i]+1);
  }
  
  ioopm_linked_list_destroy(ll);
}

/// @brief Getting values from linked list
void test_get_ll()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  int values[] = {69, -1, 0};

  CU_ASSERT_TRUE(Unsuccessful(ioopm_linked_list_get(ll, 0)));
  CU_ASSERT_TRUE(Unsuccessful(ioopm_linked_list_get(ll, -1)));
  CU_ASSERT_TRUE(Unsuccessful(ioopm_linked_list_get(ll, 1)));

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
  }

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_option_t result = ioopm_linked_list_get(ll, i);

    CU_ASSERT_TRUE(Successful(result));
    CU_ASSERT_TRUE(result.value.int_elem == values[i]);
  }

  ioopm_linked_list_destroy(ll);
}

/// @brief Inserting into empty list
void test_insert_in_empty_ll()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  
  // Index > n, ska inte gå
  ioopm_option_t result = ioopm_linked_list_insert(ll,1, Int_elem(5));
  CU_ASSERT_TRUE(Unsuccessful(result));

  result = ioopm_linked_list_insert(ll, 0, Int_elem(5));
  //Updating element to 4
  result = ioopm_linked_list_insert(ll, 0, Int_elem(4));
  CU_ASSERT_TRUE(Successful(result));

  ioopm_linked_list_destroy(ll);
}

/// @brief Inserting into list
void test_insert_ll()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  int values[] = {69, -1, 0};

  // Dåligt index
  ioopm_option_t result = ioopm_linked_list_insert(ll, -1, Int_elem(values[1]));
  CU_ASSERT_TRUE(Unsuccessful(result));
  
  // Sätt in i början av listan - [0]
  result = ioopm_linked_list_insert(ll, 0, Int_elem(values[2]));
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, values[2]);

  // Dåligt index
  result = ioopm_linked_list_insert(ll, -1, Int_elem(values[1]));
  CU_ASSERT_TRUE(Unsuccessful(result));

  // Slutet av listan - [0, -1]
  result = ioopm_linked_list_insert(ll, 1, Int_elem(values[1]));

  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, values[1]);

  // Mitten av listan - [0, 69, -1]
  result = ioopm_linked_list_insert(ll, 1, Int_elem(values[0]));
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, values[0]);

  ioopm_linked_list_destroy(ll);
}

/// @brief Clearing list
void test_clear_linked_list()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  int len = 3;
  int values[] = {69, -1, 0};

  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
  }
  CU_ASSERT_TRUE(ioopm_linked_list_size(ll) == len);

  ioopm_linked_list_clear(ll);
  CU_ASSERT_TRUE(ioopm_linked_list_size(ll) == 0);

  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
  }
  CU_ASSERT_TRUE(ioopm_linked_list_size(ll) == len);

  ioopm_linked_list_clear(ll);
  CU_ASSERT_TRUE(ioopm_linked_list_size(ll) == 0);

  ioopm_linked_list_destroy(ll);
}

/// @brief Size of linked list
void test_linked_list_size()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  int len = 3;
  int values[] = {69, -1, 0};

  CU_ASSERT_EQUAL(ioopm_linked_list_size(ll), 0);

  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
    CU_ASSERT_EQUAL(ioopm_linked_list_size(ll), i+1);
  }

  ioopm_linked_list_clear(ll);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(ll), 0); 

  ioopm_linked_list_destroy(ll);
}

/// @brief Removing links in linked list
void test_ll_remove()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  // remove from empty should just return false
  CU_ASSERT_TRUE(Unsuccessful(ioopm_linked_list_remove(ll,0)));
  int values[] = {69, -1, 0, 420};
  int len = 4;
  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
  }

  // removing from start of list - [69, 0, 420]
  ioopm_option_t result = ioopm_linked_list_remove(ll, 1);
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, -1);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(ll), len-1);

  // Remove from middle of list - [0, 420]
  result = ioopm_linked_list_remove(ll, 0);
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, 69);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(ll), len-2);

  // Remove from end of list - [0]
  result = ioopm_linked_list_remove(ll, 1); 
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, 420);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(ll), len-3);

  // Remove with only one element in list - []
  result = ioopm_linked_list_remove(ll, 0);
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, 0);
  CU_ASSERT_EQUAL(ioopm_linked_list_size(ll), 0);

  // Remove with 0 elements in list - []
  result = ioopm_linked_list_remove(ll, 4);
  CU_ASSERT_TRUE(Unsuccessful(result));
  CU_ASSERT_EQUAL(ioopm_linked_list_size(ll),0);

  ioopm_linked_list_destroy(ll);
}

/// @brief Check if predicate holds for any of the links
static bool is_funny_number(ioopm_elem_t index, ioopm_elem_t value, void *extra)
{
  return value.int_elem == 69;
}

void test_ll_any()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  int values[] = {-1, 0, 420};
  int len = 3;

  // Testar på tom lista
  CU_ASSERT_FALSE(ioopm_linked_list_any(ll, is_funny_number, NULL));

  // Första elementet 
  ioopm_linked_list_append(ll, Int_elem(69));

  CU_ASSERT_TRUE(ioopm_linked_list_any(ll, is_funny_number, NULL));

  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
    CU_ASSERT_TRUE(ioopm_linked_list_any(ll, is_funny_number, NULL));
  }

  ioopm_linked_list_destroy(ll);
}

/// @brief Check if list contains a specific value
void test_ll_contains()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  int values[] = {-1, 0, 420};
  int len = 3;

  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(values[i]));
  }
  // check value in the list
  CU_ASSERT_TRUE(ioopm_linked_list_contains(ll,Int_elem(-1)));
  // Check value not in the list  
  CU_ASSERT_FALSE(ioopm_linked_list_contains(ll,Int_elem(6)));

  ioopm_linked_list_destroy(ll);
}

/// @brief Check if predicate holds for all of the links
void test_ll_all()
{
  ioopm_list_t *ll1 = ioopm_linked_list_create(ioopm_int_eq);
  ioopm_list_t *ll2 = ioopm_linked_list_create(ioopm_int_eq);

  int values[] = {69, 69, 420};
  int len = 3;

  CU_ASSERT_FALSE(ioopm_linked_list_all(ll1, is_funny_number, NULL));
  CU_ASSERT_FALSE(ioopm_linked_list_all(ll2, is_funny_number, NULL));

  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll1, Int_elem(values[i]));
    ioopm_linked_list_append(ll2, Int_elem(69));
  }
  CU_ASSERT_FALSE(ioopm_linked_list_all(ll1, is_funny_number, NULL));
  CU_ASSERT_TRUE(ioopm_linked_list_all(ll2, is_funny_number, NULL));

  ioopm_linked_list_destroy(ll1);
  ioopm_linked_list_destroy(ll2);
}

/// @brief Apply function to all links in the list
void test_apply_to_all()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  int values[] = {69, 69, 420};
  int len = 3;

  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_linked_list_append(ll,Int_elem(values[i]));
  }

  change_to_16(ll);
  for (uint32_t i = 0; i < len; i++)
  {
    ioopm_option_t result = ioopm_linked_list_get(ll, i);
    CU_ASSERT_TRUE(Successful(result));
    CU_ASSERT_EQUAL(result.value.int_elem, 16);
  }
  
  ioopm_linked_list_destroy(ll);
}

int main() 
{
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite linked_list_tests = CU_add_suite("Testing linked list", init_suite, clean_suite);
  if (linked_list_tests == NULL) 
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
    (CU_add_test(linked_list_tests, "Creating and destroying linked list", test_create_destroy_ll) == NULL)       ||
    (CU_add_test(linked_list_tests, "Appending to linked list", test_append_ll) == NULL)                          ||
    (CU_add_test(linked_list_tests, "Prepending to linked list", test_prepend_ll) == NULL)                        ||
    (CU_add_test(linked_list_tests, "Appending and prepending to linked list", test_append_and_prepend) == NULL)  ||
    (CU_add_test(linked_list_tests, "Getting values from linked list", test_get_ll) == NULL)                      ||
    (CU_add_test(linked_list_tests, "Inserting into empty list", test_insert_in_empty_ll) == NULL)                ||
    (CU_add_test(linked_list_tests, "Inserting into list", test_insert_ll) == NULL)                               ||
    (CU_add_test(linked_list_tests, "Clearing list", test_clear_linked_list) == NULL)                             ||
    (CU_add_test(linked_list_tests, "Size of linked list", test_linked_list_size) == NULL)                        ||
    (CU_add_test(linked_list_tests, "Removing links in linked list", test_ll_remove) == NULL)                     ||
    (CU_add_test(linked_list_tests, "Check if predicate holds for any of the links", test_ll_any) == NULL)        ||
    (CU_add_test(linked_list_tests, "Check if list cotains a specific value", test_ll_contains) == NULL)          ||
    (CU_add_test(linked_list_tests, "Check if predicate holds for all of the links", test_ll_all) == NULL)        ||
    (CU_add_test(linked_list_tests, "Apply function to all links in the list", test_apply_to_all) == NULL)        ||
    0
  )
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