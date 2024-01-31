#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "../src/header/format_string/format_string.h"
int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

void test_format_string_expander_various() {
  char buffer[128];
  
  char *format_string_1 = "2c2i2l2f2d2*";
  char *expected_expansion_1 = "cciillffdd**";

  format_string_expander(format_string_1, buffer);

  CU_ASSERT(strcmp(buffer, expected_expansion_1) == 0);

  
  char *format_string_2 = "4ccl3*ccf2f";
  char *expected_expansion_2 = "cccccl***ccfff";

  format_string_expander(format_string_2, buffer);

  CU_ASSERT(strcmp(buffer, expected_expansion_2) == 0);
}

void test_format_string_contains_only_primitive_datatypes() {
  char *format_string_with_pointer = "2*cl";
  CU_ASSERT_FALSE(format_string_contains_only_primitive_datatypes(format_string_with_pointer));

  char *format_string_without_pointer = "2c2l";
  CU_ASSERT(format_string_contains_only_primitive_datatypes(format_string_without_pointer));
}

void test_format_string_contains_only_numbers() {
  char *format_string_with_numbers = "12";
  CU_ASSERT(format_string_contains_only_numbers(format_string_with_numbers));

  char *format_string_not_only_numbers = "c2*l";
  CU_ASSERT_FALSE(format_string_contains_only_numbers(format_string_not_only_numbers));
}

void test_expanded_format_string_required_space_for_data_alloc() {
  char *string_1 = "iicilic";
  int size_1 = expanded_format_string_required_space_for_data_alloc(string_1);
  int expected_size_1 = 8 * 4;
  CU_ASSERT_EQUAL(size_1, expected_size_1);

  char *string_2 = "iccciflfc";
  int size_2 = expanded_format_string_required_space_for_data_alloc(string_2);
  int expected_size_2 = 8 * 4;
  CU_ASSERT_EQUAL(size_2, expected_size_2);

  char *string_3 = "llllllccii";
  int size_3 = expanded_format_string_required_space_for_data_alloc(string_3);
  int expected_size_3 = 8 * 8;
  CU_ASSERT_EQUAL(size_3, expected_size_3);

  char *string_4 = "iicccccccc";
  int size_4 = expanded_format_string_required_space_for_data_alloc(string_4);
  int expected_size_4 = 8 * 2;
  CU_ASSERT_EQUAL(size_4, expected_size_4);
}

int main()
{
  CU_pSuite format_string_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  format_string_test_suite = CU_add_suite("Format string test suite", init_suite, clean_suite);
  if (NULL == format_string_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if ( (NULL == CU_add_test(format_string_test_suite, "Format string expander expanding various strings", test_format_string_expander_various)) ||
       (NULL == CU_add_test(format_string_test_suite, "Format string contains only primitive datatypes", test_format_string_contains_only_primitive_datatypes )) ||
       (NULL == CU_add_test(format_string_test_suite, "Format string contains only numbers", test_format_string_contains_only_numbers)) ||
       (NULL == CU_add_test(format_string_test_suite, "Calculation of the required space for a data alloc with (expanded) format string", test_expanded_format_string_required_space_for_data_alloc))

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
