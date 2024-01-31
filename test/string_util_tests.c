#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "../src/util/string_util.h"
int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}
#define ASCII_ZERO 48
void test_is_ascii_number_all_numbers() {
  char all_numbers[11] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};

  for(int i = 0; i < strlen(all_numbers); i++) {
    CU_ASSERT(is_ascii_number(all_numbers[i]));    
  }
}

void test_is_ascii_number_some_non_numbers() {
  char nonnumbers[4] = {'a', 'x', '#', '\0'};
  for(int i = 0; i < strlen(nonnumbers); i++) {
    CU_ASSERT_FALSE(is_ascii_number(nonnumbers[i]));
  }
}

//duplicate_string is tested in allocation_tests.c

int main()
{
  CU_pSuite string_util_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  string_util_test_suite = CU_add_suite("Test Suite 1", init_suite, clean_suite);
  if (NULL == string_util_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (     
      (NULL == CU_add_test(string_util_test_suite, "Testing is_ascii_numbers for all ascii-digits", test_is_ascii_number_all_numbers)) ||
      (NULL == CU_add_test(string_util_test_suite, "Testing is_ascii_numbers for some non-ascii-digits", test_is_ascii_number_some_non_numbers))
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
