#include <CUnit/Basic.h>
#include "iterator.h"

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

/// @brief Creating and destroying an iterator
void test_iterator_create_destroy()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  ioopm_list_iterator_t *iter = ioopm_list_iterator(ll);

  CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));

  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(ll);
}

/// @brief Does the iterator have an element after the current one?
void test_has_next()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);
  ioopm_list_iterator_t *iter1 = ioopm_list_iterator(ll);

  CU_ASSERT_FALSE(ioopm_iterator_has_next(iter1));

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(i));
  }

  ioopm_list_iterator_t *iter2 = ioopm_list_iterator(ll);

  CU_ASSERT_FALSE(ioopm_iterator_has_next(iter1));

  CU_ASSERT_TRUE(ioopm_iterator_has_next(iter2));

  ioopm_iterator_destroy(iter1);
  ioopm_iterator_destroy(iter2);

  ioopm_linked_list_destroy(ll);
}

/// @brief Get the next item from the iterator
void test_next()
{
  ioopm_list_t *ll = ioopm_linked_list_create(ioopm_int_eq);

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_append(ll, Int_elem(i));
  }
  
  ioopm_list_iterator_t *iter = ioopm_list_iterator(ll);

  ioopm_option_t result = ioopm_iterator_next(iter);
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, 1);

  result = ioopm_iterator_next(iter);
  CU_ASSERT_TRUE(Successful(result));
  CU_ASSERT_EQUAL(result.value.int_elem, 2);
  
  result = ioopm_iterator_next(iter);
  CU_ASSERT_TRUE(Unsuccessful(result));

  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(ll);
}

/// @brief Resetting the iterator
void test_reset()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq);

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_append(list, Int_elem(i));
  }

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

  int first_value = ioopm_iterator_next(iter).value.int_elem;

  ioopm_iterator_reset(iter);

  int reseted_first_value = ioopm_iterator_next(iter).value.int_elem;
  CU_ASSERT_EQUAL(first_value, reseted_first_value);
  
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

/// @brief Get current value from iterator
void test_current()
{
  ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq);

  for (uint32_t i = 0; i < 3; i++)
  {
    ioopm_linked_list_append(list, Int_elem(i));
  }

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  
  for (uint32_t i = 0; i < 3; i++)
  {
    CU_ASSERT_EQUAL(ioopm_iterator_current(iter).int_elem, i);
    ioopm_iterator_next(iter);
  }
  
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

int main() 
{
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite iterator_tests = CU_add_suite("Testing iterators on a linked list", init_suite, clean_suite);
  if (iterator_tests == NULL) 
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
    (CU_add_test(iterator_tests, "Creating and destroying an iterator", test_iterator_create_destroy) == NULL) ||
    (CU_add_test(iterator_tests, "Does the iterator have an element after the current one?", test_has_next) == NULL) ||
    (CU_add_test(iterator_tests, "Get the next item from the iterator", test_next) == NULL) ||
    (CU_add_test(iterator_tests, "Resetting the iterator", test_reset) == NULL) ||
    (CU_add_test(iterator_tests, "Get current value from iterator", test_current) == NULL) ||
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
