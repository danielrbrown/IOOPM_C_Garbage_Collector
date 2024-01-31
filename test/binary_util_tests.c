#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include "../src/util/binary_util.h"
int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

void test_binary_string_to_int_zero() {
  char *binary_string_zero = "0000000000000000000000000000000000000000000000000000000000000000";
  uint64_t binary_string_zero_int_value = 0;
  CU_ASSERT_EQUAL(binary_string_to_uint64(binary_string_zero), binary_string_zero_int_value);
}

void test_binary_string_to_int_various_numbers() {
  char *binary_string_1 = "0000000000000000000000000000000000000000000000000000000000000001";
  uint64_t binary_string_1_int_value = 1;
  //printf("%lu", binary_string_to_uint64(binary_string_1));
  CU_ASSERT_EQUAL(binary_string_to_uint64(binary_string_1), binary_string_1_int_value);

  char *binary_string_2 = "0000000000000000000000000000000000000000000000000000000000101010";
  uint64_t binary_string_2_int_value = 42;
  CU_ASSERT_EQUAL(binary_string_to_uint64(binary_string_2), binary_string_2_int_value);

  char *binary_string_3 = "0000000000000000000000000000000000000000000000000000000001000101";
  uint64_t binary_string_3_int_value = 69;
  CU_ASSERT_EQUAL(binary_string_to_uint64(binary_string_3), binary_string_3_int_value);
}

// void test_binary_string_to_int_msb() {
//   char *binary_string = "1000000000000000000000000000000000000000000000000000000000000000";
//   uint64_t binary_string_int_value = 9223372036854775808;
//   CU_ASSERT_EQUAL(binary_string_to_uint64(binary_string), binary_string_int_value);
// }
  
int main()
{
  CU_pSuite binary_util_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  binary_util_test_suite = CU_add_suite("Binary util test suite", init_suite, clean_suite);
  if (NULL == binary_util_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(binary_util_test_suite, "Binary to string all zeroes", test_binary_string_to_int_zero)) ||
      (NULL == CU_add_test(binary_util_test_suite, "Binary to string various numbers", test_binary_string_to_int_various_numbers))
      // (NULL == CU_add_test(binary_util_test_suite, "Binary to string where binary number uses MSB", test_binary_string_to_int_msb))
      
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