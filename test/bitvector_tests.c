#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include "../src/util/binary_util.h"
#include "../src/bitvector/bitvector.h"

#define BITVECTOR_ALL_ONES 0xffffffffffffffff
int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

//function copypasted from binary_util.c. Might be better to put the function in a math util module at some point
static uint64_t myPow(uint64_t base, int exponent) {
  uint64_t result = 1;
  
  for(int i = 0; i < exponent; i++) {
    result *= base;
  }
  
  return result;
}
//----------------

void test_bitvector_create() {
  bitvector_t bitvector_0 = bitvector_create(0);
  bitvector_t bitvector_42 = bitvector_create(42);
  bitvector_t bitvector_max = bitvector_create(BITVECTOR_ALL_ONES);
  CU_ASSERT_EQUAL((uint64_t) bitvector_0, (uint64_t) 0);
  CU_ASSERT_EQUAL((uint64_t) bitvector_42, (uint64_t) 42);
  CU_ASSERT_EQUAL((uint64_t) bitvector_max, (uint64_t) BITVECTOR_ALL_ONES);
}

void test_bitvector_array_create() {
  int length = 4;
  bitvector_t bitvector_array[length];
  uint64_t initial_values[4] = {0, 23, 42, 99};
  bitvector_array_create(bitvector_array, initial_values, length);
  for (int i = 0; i < length; i++) {
    CU_ASSERT_EQUAL(bitvector_array[i], initial_values[i]);
  }
}

void set_bits_to_true(bitvector_t *bitvector) {
  for(int i = 0; i < 64; i++) {
    bitvector_t bitvector = 0;
    uint64_t expected_value = 0;
    
    for(int j = 0; j < i; j++) {
      expected_value += myPow(2, j);
      bitvector_set_bit(&bitvector, j, true);
    }
    
    CU_ASSERT_EQUAL(bitvector, expected_value);
    }
}

void test_set_bits_to_true() {
  bitvector_t bitvector = bitvector_create(0);
  set_bits_to_true(&bitvector);
}

void test_array_set_bits_to_true() {
  int length = 10;
  bitvector_t bitvector_array[length];
  uint64_t initial_values[10] = {0};
  bitvector_array_create(bitvector_array, initial_values, length);
  for(int i = 0; i < length; i++) {
    set_bits_to_true(&bitvector_array[i]);
  }
}

void set_bits_to_false(bitvector_t bitvector) {
  for(int i = 0; i < 64; i++) {
    bitvector_set_bit(&bitvector, i, false);
    
    uint64_t expected_value = 0;    
    for(int j = 63; j > i; j--) {
      expected_value += myPow(2, j);
    }
 
    CU_ASSERT_EQUAL(bitvector, expected_value);
  }
}


void test_set_bits_to_false() {
  bitvector_t bitvector = BITVECTOR_ALL_ONES;
  set_bits_to_false(bitvector);
}

void test_array_set_bits_to_false() {
  int length = 4;
  bitvector_t bitvector_array[length];
  uint64_t initial_values[4];
  for (int i = 0; i < length; i++) {
    initial_values[i] = (uint64_t) BITVECTOR_ALL_ONES;
  }
  bitvector_array_create(bitvector_array, initial_values, length);
  for(int i = 0; i < length; i++) {
    set_bits_to_false(bitvector_array[i]);
  }
}

void read_bit_all_zeroes(bitvector_t *bitvector) {
  for(int i = 0; i < 64; i++) {
    CU_ASSERT_FALSE(bitvector_read_bit(*bitvector, i));
  }
}

void read_bit_all_ones(bitvector_t *bitvector) {
  for(int i = 0; i < 64; i++) {
    CU_ASSERT(bitvector_read_bit(*bitvector, i));
  }
}

void test_read_bit_all_zeroes() {
  bitvector_t bv_all_zeroes = 0;
  read_bit_all_zeroes(&bv_all_zeroes);
}

void test_array_read_bit_all_zeroes() {
  int length = 4;
  bitvector_t bv_all_zeroes[length];
  uint64_t initial_values[4] = {0, 0, 0, 0};
  bitvector_array_create(bv_all_zeroes, initial_values, length);
  for(int i = 0; i < length; i++) {
    read_bit_all_zeroes(&bv_all_zeroes[i]);
  }
}

void test_read_bit_all_ones() {
  bitvector_t bv_all_ones = BITVECTOR_ALL_ONES;
  read_bit_all_ones(&bv_all_ones);
}

void test_array_read_bit_all_ones() {
  int length = 4;
  bitvector_t bv_all_zeroes[length];
  uint64_t initial_values[4];
  for (int i = 0; i < length; i++) {
    initial_values[i] = (uint64_t) BITVECTOR_ALL_ONES;
  }
  bitvector_array_create(bv_all_zeroes, initial_values, length);
  for(int i = 0; i < length; i++) {
    read_bit_all_ones(&bv_all_zeroes[i]);
  }
}


void test_bitvector_count_ones_various() {
  bitvector_t bv_all_ones = BITVECTOR_ALL_ONES;
  CU_ASSERT_EQUAL(bitvector_count_ones(bv_all_ones), 64);

  bitvector_t bv_all_zeroes = 0;
  CU_ASSERT_EQUAL(bitvector_count_ones(bv_all_zeroes), 0);

  bitvector_t bv_half_are_ones = binary_string_to_uint64("0000000000000000000000000000000011111111111111111111111111111111");
  CU_ASSERT_EQUAL(bitvector_count_ones(bv_half_are_ones), 32);
}

void test_bitvector_array_count_ones_various() {
  bitvector_t bitvector_array_0[4];
  uint64_t initial_values_0[4] = {3, 7, 99, 23};
  bitvector_array_create(bitvector_array_0, initial_values_0, 4);

  CU_ASSERT_EQUAL(bitvector_array_count_ones(bitvector_array_0, 4), 13);

  bitvector_t bitvector_array_1[4];
  uint64_t initial_values_1[4] = {0};
  bitvector_array_create(bitvector_array_1, initial_values_1, 4);

  CU_ASSERT_EQUAL(bitvector_array_count_ones(bitvector_array_1, 4), 0);
}


  
int main()
{
  CU_pSuite bitvector_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  bitvector_test_suite = CU_add_suite("Bitvector test suite", init_suite, clean_suite);
  if (NULL == bitvector_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(bitvector_test_suite, "Test bitvector creation, various initialisations", test_bitvector_create)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Test bitvector array creation, various initialisations", test_bitvector_array_create)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Set bits on all indices from false to true",test_set_bits_to_true )) ||
      (NULL == CU_add_test(bitvector_test_suite, "Set bits on all indices in an array from false to true",test_array_set_bits_to_true )) ||
      (NULL == CU_add_test(bitvector_test_suite, "Set bits on all indices in an array from true to false",test_array_set_bits_to_false )) ||
      (NULL == CU_add_test(bitvector_test_suite, "Set bits on all indices from true to false", test_set_bits_to_false)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Read bits where bitvector is all zeroes", test_read_bit_all_zeroes)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Read bits where bitvector array is all zeroes", test_array_read_bit_all_zeroes)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Read bits where bitvector array is all ones", test_array_read_bit_all_ones)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Read bits where bitvector is all ones", test_read_bit_all_ones)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Count zeroes in different bitvectors", test_bitvector_count_ones_various)) ||
      (NULL == CU_add_test(bitvector_test_suite, "Count zeroes in arrays of bitvectors", test_bitvector_array_count_ones_various))
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