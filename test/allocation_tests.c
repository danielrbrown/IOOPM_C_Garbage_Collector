#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "../src/gc.h"
#include "../src/page_map/page_map.h"
#include "../src/allocate/allocate.h"

#define AMOUNT_OF_PAGES 8
#define PAGEMAP_SIZE 1
#define HEAP_SIZE (h_metadata_size() + (p_metadata_size() + PAGESIZE) * AMOUNT_OF_PAGES) + PAGEMAP_SIZE
#define UNSAFE_STACK true
#define GC_THRESHOLD 1
#define FIRST_PAGE_ALLOCATION_PTR (void *)(((unsigned long)p_start_address(h_first_page(heap))) + sizeof(header_t))

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

//strdup fast för våran heap
static char *duplicate_string(heap_t *heap, char *string) {
  int array_size = strlen(string) + 1;
  char *allocated_string = h_alloc_data(heap, array_size * sizeof(char));

  for(int i = 0; i <= array_size; i++) {
    allocated_string[i] = string[i];
  }

  return allocated_string;
}

void test_allocate_single_character() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  char c = 'a';

  char *allocated_c = h_alloc_data(heap, sizeof(char));
  *allocated_c = c;

  CU_ASSERT_EQUAL(*allocated_c, c);



  h_delete(heap);
}

void test_allocate_single_characters_and_check_padding() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);

  char c1 = 'a';
  char *allocated_c1 = h_alloc_data(heap, sizeof(char));
  *allocated_c1 = c1;

  char c2 = 'b';
  char *allocated_c2 = h_alloc_data(heap, sizeof(char));
  *allocated_c2 = c2;


  CU_ASSERT_EQUAL(*allocated_c1, c1);
  CU_ASSERT_EQUAL(*allocated_c2, c2);

  //Antar att allokeringen sker på samma page vilket den borde göra i nuvarande implementationen
  for(int i = 1; i < 8; i++) {
    char padding = allocated_c1[i];
    CU_ASSERT_EQUAL(padding, 0);
  }

  CU_ASSERT_PTR_EQUAL(allocated_c2, &allocated_c1[16]);

  h_delete(heap);
}

void test_allocate_string() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  char *string = "bazinga";
  char *allocated_string = duplicate_string(heap, string);

  CU_ASSERT(strcmp(allocated_string, string) == 0);

  h_delete(heap);
}

void test_allocate_fill_page() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  
  for(int i = 0; i < 128; i++) {
    h_alloc_struct(heap, "*");
  }
  
  void *pointer_on_second_page = h_alloc_struct(heap, "2*");
  page_t *second_page = h_get_page(heap, 1);
  void *expected_location = (void *)((uint64_t)p_start_address(second_page) + sizeof(header_t));

  CU_ASSERT_EQUAL(pointer_on_second_page, expected_location);

  h_delete(heap);
}

void test_allocate_integer() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  int int1 = 123456789;
  int *allocated_int1 = h_alloc_data(heap, sizeof(int));
  *allocated_int1 = int1;
  CU_ASSERT_EQUAL(*allocated_int1, int1);

  int int2 = 987654321;
  int *allocated_int2 = h_alloc_data(heap, sizeof(int));
  *allocated_int2 = int2;
  CU_ASSERT_EQUAL(*allocated_int2, int2);

  CU_ASSERT_EQUAL((void *)allocated_int2, (void *)((uint64_t)allocated_int1 + 16));

  h_delete(heap);
}

void test_allocate_struct(){
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  char char1 = 'F';
  char *allocated_bytes = h_alloc_struct(heap, "50");
  *allocated_bytes = char1;
  CU_ASSERT_EQUAL(*allocated_bytes, char1);
  h_delete(heap);
}

void test_allocate_struct_int() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  int int1 = 1122211;
  int *allocated_ptr = h_alloc_struct(heap, "*i");
  *allocated_ptr = int1;
  CU_ASSERT_EQUAL(*allocated_ptr, int1);
  CU_ASSERT_PTR_EQUAL(FIRST_PAGE_ALLOCATION_PTR, allocated_ptr);
  h_delete(heap);
}

void test_allocate_struct_char(){
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  char char1 = 'c';
  char *allocated_ptr = h_alloc_struct(heap, "*c");
  *allocated_ptr = char1;
  CU_ASSERT_EQUAL(*allocated_ptr, char1);

  void *expected_address = (void *)((uint64_t)FIRST_PAGE_ALLOCATION_PTR + 8 * 2);
  CU_ASSERT_PTR_EQUAL(expected_address, allocated_ptr);
  h_delete(heap);
}

void test_allocate_struct_long() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  long long1 = 125345456;
  long *allocated_ptr = h_alloc_struct(heap, "*l");
  *allocated_ptr = long1;
  CU_ASSERT_EQUAL(*allocated_ptr, long1);
  CU_ASSERT_PTR_EQUAL(FIRST_PAGE_ALLOCATION_PTR, allocated_ptr);
  h_delete(heap);
}

void test_allocate_struct_double() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  double double1 = 4334333;                //Note that the maximum space is 2 numbers meaning 123 doesnt fit on the allocated area
  double *allocated_ptr = h_alloc_struct(heap, "*d");
  *allocated_ptr = double1;
  CU_ASSERT_EQUAL(*allocated_ptr, double1);
  CU_ASSERT_PTR_EQUAL(FIRST_PAGE_ALLOCATION_PTR, allocated_ptr);
  h_delete(heap);
}

void test_allocate_struct_float() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  float float1 = 433.567;
  float *allocated_ptr = h_alloc_struct(heap, "*3f");
  *allocated_ptr = float1;
  CU_ASSERT_EQUAL(*allocated_ptr, float1);
  CU_ASSERT_PTR_EQUAL(FIRST_PAGE_ALLOCATION_PTR, allocated_ptr);
  h_delete(heap);
}

void test_allocate_float() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  float float1 = 433.567;
  float *allocated_ptr = h_alloc_struct(heap, "3f");
  *allocated_ptr = float1;
  CU_ASSERT_EQUAL(*allocated_ptr, float1);
  CU_ASSERT_PTR_EQUAL(FIRST_PAGE_ALLOCATION_PTR, allocated_ptr);
  h_delete(heap);
}

void test_allocate_long_struct() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  int *allocated_ptr = h_alloc_struct(heap, "3i2c");

  int int1 = 11111;
  int int2 = 123456789;
  int int3 = 22222;

  CU_ASSERT_PTR_EQUAL(FIRST_PAGE_ALLOCATION_PTR, allocated_ptr);

  *allocated_ptr = int1;

  int *allocated_ptr2 = allocated_ptr + 1;
  *allocated_ptr2 = int2;

  int *allocated_ptr3 = allocated_ptr2 + 1;
  *allocated_ptr3 = int3;

  CU_ASSERT_EQUAL(allocated_ptr[0], *allocated_ptr);
  CU_ASSERT_EQUAL(allocated_ptr[1], *allocated_ptr2);
  CU_ASSERT_EQUAL(allocated_ptr[2], *allocated_ptr3);

  char char1 = 'a';
  char char2 = 'b';

  int *allocated_ptr4 = allocated_ptr3 + 1;
  *allocated_ptr4 = char1;

  int *allocated_ptr5 = allocated_ptr4 + 1;
  *allocated_ptr5 = char2;

  CU_ASSERT_EQUAL(allocated_ptr[3], *allocated_ptr4);
  CU_ASSERT_EQUAL(allocated_ptr[4], *allocated_ptr5);

  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + sizeof(int)), allocated_ptr2);
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + 2*sizeof(int)), allocated_ptr3);
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + 3*sizeof(int)), allocated_ptr4);
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + 4*sizeof(int)), allocated_ptr5);

  h_delete(heap);
}

void test_allocate_long_struct_real() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);
  struct ints_and_chars *allocated_ptr = h_alloc_struct(heap, "3i2c");

  struct ints_and_chars {
    int int1;
    int int2;
    int int3;
    char char1;
    char char2;
  };

  struct ints_and_chars data;
  data.int1 = 11111;
  data.int2 = 123456789;
  data.int3 = 22222;
  data.char1 = 'a';
  data.char2 = 'b';

  *allocated_ptr = data;

  CU_ASSERT_PTR_EQUAL(FIRST_PAGE_ALLOCATION_PTR, allocated_ptr);
  CU_ASSERT_PTR_EQUAL(allocated_ptr, (void *)&(allocated_ptr->int1));

  // Check that struct elements are where we expect them in the heap
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR)), (void *)&(allocated_ptr->int1));
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + sizeof(int)), (void *)&(allocated_ptr->int2));
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + 2*sizeof(int)), (void *)&(allocated_ptr->int3));
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + 3*sizeof(int)), (void *)&(allocated_ptr->char1));
  CU_ASSERT_PTR_EQUAL((void *)(((unsigned long)FIRST_PAGE_ALLOCATION_PTR) + 3*sizeof(int) + sizeof(char)), (void *)&(allocated_ptr->char2));

  // Make an additional allocation after the struct allocation and check that this is allocated at the expected address
  h_alloc_data(heap, sizeof(int));

  CU_ASSERT_EQUAL(allocated_ptr->char2, 'b');
  
  h_delete(heap);
}

void test_struct_padding_for_datatypes_of_different_sizes() {
  heap_t *heap = h_init(HEAP_SIZE, UNSAFE_STACK, GC_THRESHOLD);

  struct strange {
    int int1;
    char char1;
    int int2;
    
    void *ptr1;
    char char2;
    void *ptr2;

    int int3;
    void *ptr3;

    char char4;
    char char5;
  };
  typedef struct strange strange_t;

  char *strange_format_string = "ici*c*i*cc";
  
  strange_t *strange = h_alloc_struct(heap, strange_format_string);

  char *int_binary_string = "0000000000000000000000000000000011000000000000000000000000000011";
  int default_int = (int)binary_string_to_uint64(int_binary_string);

  char *char_binary_string = "0000000000000000000000000000000000000000000000000000000001010101";
  int default_char = (char)binary_string_to_uint64(char_binary_string);

  char *pointer_binary_string = "1111000000000000000000000000000000000000000000000000000000001111";
  void *default_ptr = (void *)binary_string_to_uint64(pointer_binary_string);

  *strange = (strange_t) {.int1 = default_int, .char1 = default_char, .int2 = default_int,
			  .ptr1 = default_ptr, .char2 = default_char, .ptr2 = default_ptr,
			  .int3 = default_int, .ptr3 = default_ptr, .char4 = default_char, .char5 = default_char};
  

  void *new_alloc = h_alloc_data(heap, 6);

  //Because bitvector layout doesn't know how large each field is, we have to assume it's the
  //largest possible (8 bytes). This means that the next allocation will be 8 * amount_of_fields + sizeof(header_t)
  CU_ASSERT_EQUAL(strange->ptr3, default_ptr);
  CU_ASSERT_PTR_EQUAL(new_alloc, (void *)((uint64_t)strange + 8 * 10 + sizeof(header_t)));
  
  h_delete(heap);
}

	     
  

  
int main()
{
  CU_pSuite allocation_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  allocation_test_suite = CU_add_suite("Allocation test suite", init_suite, clean_suite);
  if (NULL == allocation_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(allocation_test_suite, "Allocate string", test_allocate_string)) ||
      (NULL == CU_add_test(allocation_test_suite, "Allocate single character", test_allocate_single_character)) ||
      (NULL == CU_add_test(allocation_test_suite, "Allocate single characters and check that there is padding", test_allocate_single_characters_and_check_padding)) ||
      (NULL == CU_add_test(allocation_test_suite, "Allocate to fill a page and check behaviour", test_allocate_fill_page)) ||
      (NULL == CU_add_test(allocation_test_suite, "Allocate single characters and check that there is padding", test_allocate_single_characters_and_check_padding))||
      (NULL == CU_add_test(allocation_test_suite, "Allocate integers and check there is padding", test_allocate_integer))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation", test_allocate_struct))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for *int", test_allocate_struct_int))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for *char", test_allocate_struct_char))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for *long",test_allocate_struct_long))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for *double",test_allocate_struct_double))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for *float",test_allocate_struct_float))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for float",test_allocate_float))||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for long struct",test_allocate_long_struct)) ||
      (NULL == CU_add_test(allocation_test_suite, "Struct allocation for long struct created by user",test_allocate_long_struct_real)) ||
      (NULL == CU_add_test(allocation_test_suite, "Struct with many different sizes of datatype fields in various orders", test_struct_padding_for_datatypes_of_different_sizes))

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
