#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "../src/header/header.h"
#include "../src/bitvector/bitvector.h"
#include "../src/util/binary_util.h"
#include <stdint.h>
#include <assert.h>
#include "../src/gc.h"

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

//copy-paste från bitvector tests. Bryta ut till egen mattemodul? -------
int64_t myPow(int base, int exponent) {
  int64_t result = 1;
  
  for(int i = 0; i < exponent; i++) {
    result *= base;
  }
  
  return result;
}
//------------
      

void test_create_header_struct_pointers_only() {
  char *format_string = "**";
  header_t header = header_creation_layout(format_string, NULL);
  char *binary_string = "1100000000000000000000000000000000000000000000000000000000010111";
  assert(strlen(binary_string) == 64);

  CU_ASSERT_EQUAL(header, binary_string_to_uint64(binary_string)); //3rd LSB decides whether it's only size or bitvektor representing where pointers are. I chose 1 for header representing bitvektor and else 0
  //LSB 4-9 used for vektor size (6 bits)
}

void test_create_header_struct_raw_data_only() {
  char *format_string = "ii";
  header_t header = header_creation_layout(format_string, NULL);
  char *binary_string = "0000000000000000000000000000000000000000000000000000000001000011";
  assert(strlen(binary_string) == 64);
  
  CU_ASSERT_EQUAL(header, binary_string_to_uint64(binary_string));
}

void test_create_header_struct_both_raw_and_pointers() {
  char *format_string = "*i**i";
  header_t header = header_creation_layout(format_string, NULL);
  char *binary_string = "1011000000000000000000000000000000000000000000000000000000101111";
  assert(strlen(binary_string) == 64);
  
  CU_ASSERT_EQUAL(header, binary_string_to_uint64(binary_string));
}

void test_create_header_struct_format_strings_synonymous() {
  char *format_string_no_numbers = "***";
  char *format_string_with_numbers = "3*";
  header_t header_no_numbers = header_creation_layout(format_string_no_numbers, NULL);
  header_t header_with_numbers = header_creation_layout(format_string_with_numbers, NULL);
  CU_ASSERT_EQUAL(header_no_numbers, header_with_numbers);

  char *format_string_no_numbers_2 = "*";
  char *format_string_with_numbers_2 = "1*";
  header_t header_no_numbers_2 = header_creation_layout(format_string_no_numbers_2, NULL);
  header_t header_with_numbers_2 = header_creation_layout(format_string_with_numbers_2, NULL);
  CU_ASSERT_EQUAL(header_no_numbers_2, header_with_numbers_2);
  
  char *format_string_no_numbers_raw = "iii";
  char *format_string_with_numbers_raw = "3i";
  header_t header_no_numbers_raw = header_creation_layout(format_string_no_numbers_raw, NULL);
  header_t header_with_numbers_raw = header_creation_layout(format_string_with_numbers_raw, NULL);
  CU_ASSERT_EQUAL(header_no_numbers_raw, header_with_numbers_raw);
}

#define AMOUNT_OF_PAGES 8
#define PAGEMAP_SIZE 1
#define HEAP_SIZE (h_metadata_size() + (p_metadata_size() + PAGESIZE) * AMOUNT_OF_PAGES) + PAGEMAP_SIZE
#define UNSAFE_STACK true
#define GC_THRESHOLD 1

void test_create_header_struct_when_format_string_has_to_be_allocated() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  char *huge_format_string = "100*"; //has to be above 55 elements
  header_t header = header_creation_layout(huge_format_string, heap);

  char *format_string_on_heap = (char *)header;
  CU_ASSERT(strcmp(format_string_on_heap, huge_format_string) == 0);

  h_delete(heap);
}

void test_create_header_raw_various_sizes() {
  header_t header_1 = header_creation_layout("8", NULL);
  char *header_1_binary_string = "0000000000000000000000000000000000000000000000000000000001000011";
  CU_ASSERT_EQUAL(header_1, binary_string_to_uint64(header_1_binary_string));

  header_t header_2 = header_creation_layout("64", NULL);
  char *header_2_binary_string = "0000000000000000000000000000000000000000000000000000001000000011";
  CU_ASSERT_EQUAL(header_2, binary_string_to_uint64(header_2_binary_string));

  header_t header_3 = header_creation_layout("128", NULL);
  char *header_3_binary_string = "0000000000000000000000000000000000000000000000000000010000000011";
  CU_ASSERT_EQUAL(header_3, binary_string_to_uint64(header_3_binary_string));
}

void test_header_interpretation_bitvector_layout_simple() {
  char *format_string = "**ii";
  header_t header = header_creation_layout(format_string, NULL);

  int offset_array[16];
  int amount_of_pointers = header_interpretation(header, offset_array);
  CU_ASSERT_EQUAL(amount_of_pointers, 2);
  for(int i = 0; i < amount_of_pointers; i++) {
    CU_ASSERT_EQUAL(offset_array[i], i * 8);
  }
}

void test_header_interpretation_bitvector_layout_struct_with_padding() {
  char *format_string = "*i*i*i";
  header_t header = header_creation_layout(format_string, NULL);

  int offset_array[16];
  int amount_of_pointers = header_interpretation(header, offset_array);
  CU_ASSERT_EQUAL(amount_of_pointers, 3);
  for(int i = 0; i < amount_of_pointers; i++) {
    CU_ASSERT_EQUAL(offset_array[i], i * 16);
  }
}

void test_header_interpretation_bitvector_layout_struct_all_kinds_of_datatypes() {
  char *format_string = "*if*ld*";
  header_t header = header_creation_layout(format_string, NULL);

  int offset_array[16];
  int amount_of_pointers = header_interpretation(header, offset_array);
  CU_ASSERT_EQUAL(amount_of_pointers, 3);
  CU_ASSERT_EQUAL(offset_array[0], 0);
  CU_ASSERT_EQUAL(offset_array[1], 16);
  CU_ASSERT_EQUAL(offset_array[2], 40);
}

void test_header_interpretation_bitvector_layout_struct_various() {
  char *format_string = "i*l*id**f*";
  header_t header = header_creation_layout(format_string, NULL);

  int offset_array[16];
  int amount_of_pointers = header_interpretation(header, offset_array);
  CU_ASSERT_EQUAL(amount_of_pointers, 5);
  CU_ASSERT_EQUAL(offset_array[0], 8);
  CU_ASSERT_EQUAL(offset_array[1], 24);
  CU_ASSERT_EQUAL(offset_array[2], 48);
  CU_ASSERT_EQUAL(offset_array[3], 56);
  CU_ASSERT_EQUAL(offset_array[4], 72);
}
void test_create_forwarding_pointer_header() {
  char *forwarding_pointer_string = "0000000000000000000000000000000000000000000000000000000000001100";
  void *forwarding_pointer = (void *)binary_string_to_uint64(forwarding_pointer_string);
  
  header_t header = create_header_with_forwarding_pointer(forwarding_pointer);
  char *header_string = "0000000000000000000000000000000000000000000000000000000000001101";
  
  CU_ASSERT_EQUAL(header, binary_string_to_uint64(header_string));
 
}

void test_size_of_raw_allocation() {
  header_t header_for_raw_allocation = header_creation_layout("8", NULL);
  CU_ASSERT_EQUAL(size_of_raw_allocation(header_for_raw_allocation), 8);
}

void test_size_of_struct_allocation() {
  header_t header_for_struct_allocation = header_creation_layout("**i", NULL);
  CU_ASSERT_EQUAL(size_of_struct_allocation(header_for_struct_allocation), 24);
}

void test_get_forwarding_pointer_from_header() {
  char *forwarding_pointer_string = "0000000000000000000000000000000000000000000000000000000000001100";
  header_t header_for_forwarding = create_header_with_forwarding_pointer((void *)binary_string_to_uint64(forwarding_pointer_string));
  void *forwarding_pointer_extracted = get_forwarding_pointer_from_header(header_for_forwarding);
  void *forwarding_pointer_expected = (void *)binary_string_to_uint64(forwarding_pointer_string);
  CU_ASSERT_EQUAL(forwarding_pointer_extracted, forwarding_pointer_expected);
}

void test_create_header_for_contstant_format_string() {
  header_t header = header_creation_layout("32", NULL);
  char *binary_string = "0000000000000000000000000000000000000000000000000000000100000011";
  CU_ASSERT_EQUAL(header, binary_string_to_uint64(binary_string));
}

void test_header_interpretation_format_string() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);

  char *format_string = "56*12i";
  void *allocation = h_alloc_struct(heap, format_string);
  header_t *header = (header_t *)((uint64_t)allocation - sizeof(header_t));

  int offset_array[128];
  size_t amount_of_pointers = header_interpretation(*header, offset_array);
  for(int i = 0; i < amount_of_pointers; i++) {
    int current_offset = offset_array[i];
    CU_ASSERT_EQUAL(current_offset, i*8);
  }
  
  h_delete(heap);
}

void test_identify_header_all_kinds() {
  header_t header_for_raw = header_creation_layout("8", NULL);
  
  header_t header_for_struct = header_creation_layout("**", NULL);
  
  char *forwarding_pointer_string = "0000000000000000000000000000000000000000000000000000000000001100";
  header_t header_for_forwarding = create_header_with_forwarding_pointer((void *)binary_string_to_uint64(forwarding_pointer_string));
  
  char *format_string_pointer_string = "0000000000000000000000000111000000000000000000000000000000000000";
  header_t format_string_header = (header_t)binary_string_to_uint64(format_string_pointer_string);

  //raw layout
  header_t h_tmp = header_for_raw;
  CU_ASSERT(is_raw_layout_header(h_tmp));
  CU_ASSERT_FALSE(is_struct_layout_header(h_tmp));
  CU_ASSERT_FALSE(is_forwarding_address_header(h_tmp));
  CU_ASSERT_FALSE(is_format_string_header(h_tmp));

  //struct layout
  h_tmp = header_for_struct;
  CU_ASSERT_FALSE(is_raw_layout_header(h_tmp));
  CU_ASSERT(is_struct_layout_header(h_tmp));
  CU_ASSERT_FALSE(is_forwarding_address_header(h_tmp));
  CU_ASSERT_FALSE(is_format_string_header(h_tmp));

  //forwarding
  h_tmp = header_for_forwarding;
  CU_ASSERT_FALSE(is_raw_layout_header(h_tmp));
  CU_ASSERT_FALSE(is_struct_layout_header(h_tmp));
  CU_ASSERT(is_forwarding_address_header(h_tmp));
  CU_ASSERT_FALSE(is_format_string_header(h_tmp));

  //format string
  h_tmp = format_string_header;
  CU_ASSERT_FALSE(is_raw_layout_header(h_tmp));
  CU_ASSERT_FALSE(is_struct_layout_header(h_tmp));
  CU_ASSERT_FALSE(is_forwarding_address_header(h_tmp));
  CU_ASSERT(is_format_string_header(h_tmp));
}

int main()
{
  CU_pSuite header_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  header_test_suite = CU_add_suite("Header tests", init_suite, clean_suite);
  if (NULL == header_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if ( (NULL == CU_add_test(header_test_suite, "Create header for struct containing only pointers", test_create_header_struct_pointers_only)) ||
       (NULL == CU_add_test(header_test_suite, "Create header for struct containing only raw data", test_create_header_struct_raw_data_only )) ||
       (NULL == CU_add_test(header_test_suite, "Create header for struct containing a mix of pointers and raw data", test_create_header_struct_both_raw_and_pointers )) ||
       (NULL == CU_add_test(header_test_suite, "Check if creating headers for struct using format strings that should be synonymous results in equal results", test_create_header_struct_format_strings_synonymous)) ||
       (NULL == CU_add_test(header_test_suite, "Create header for struct when format string has to be allocated", test_create_header_struct_when_format_string_has_to_be_allocated)) ||
       (NULL == CU_add_test(header_test_suite, "Create header for raw data of various sizes", test_create_header_raw_various_sizes )) ||
       (NULL == CU_add_test(header_test_suite, "Interpretation of simple bitvector layout header", test_header_interpretation_bitvector_layout_simple)) ||
       (NULL == CU_add_test(header_test_suite, "Interpretation of bitvector layout where padding exists in the struct", test_header_interpretation_bitvector_layout_struct_with_padding )) ||
       (NULL == CU_add_test(header_test_suite, "Interpretation of bitvector layout for struct that uses all supported primitve datatypes", test_header_interpretation_bitvector_layout_struct_all_kinds_of_datatypes)) ||
       (NULL == CU_add_test(header_test_suite, "Interpretation of bitvector layout header for various struct(s)", test_header_interpretation_bitvector_layout_struct_various)) ||
       (NULL == CU_add_test(header_test_suite, "Create a forwarding pointer header", test_create_forwarding_pointer_header)) ||
       (NULL == CU_add_test(header_test_suite, "Size of raw allocation", test_size_of_raw_allocation)) ||
       (NULL == CU_add_test(header_test_suite, "Size of struct allocation", test_size_of_struct_allocation)) ||
       (NULL == CU_add_test(header_test_suite, "Extract forwarding pointer from header", test_get_forwarding_pointer_from_header)) ||
       (NULL == CU_add_test(header_test_suite, "Creating header for format string that only contains numbers", test_create_header_for_contstant_format_string)) ||
       (NULL == CU_add_test(header_test_suite, "Interpret struct header where header is allocated format string", test_header_interpretation_format_string)) ||
       (NULL == CU_add_test(header_test_suite, "Check that all header idenitification function works correctly", test_identify_header_all_kinds))
      
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
