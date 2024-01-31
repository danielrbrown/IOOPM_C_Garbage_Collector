#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include "../src/gc.h"
#include "../src/allocation_map/allocation_map.h"
#include "../src/page_map/page_map.h"

#define PAGES_192 (P_ALLOC_SIZE * 192 + h_metadata_size() + sizeof(bitvector_t) * 3) // Space for 192 pages and three full bitvectors in pagemap
#define PAGES_191 (P_ALLOC_SIZE * 191 + h_metadata_size() + sizeof(bitvector_t) * 3) // Space for 191 pages and three bitvector in pagemap

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

// PAGE MAP TESTS

void test_pagemap_initialisation() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);
  int no_pages = h_page_amount(heap);
  
  for (int i = 0; i < no_pages; i++) {
    CU_ASSERT_FALSE(pm_get_page_status(heap, i));
  }
  h_delete(heap);
}

void test_pagemap_set_all_pages_to_active() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);
  int no_pages = h_page_amount(heap);
  
  for (int i = 0; i < no_pages; i++) {
    pm_set_page_to(heap, i, true);
    CU_ASSERT_TRUE(pm_get_page_status(heap, i));
  }
  for (int i = 0; i < no_pages; i++) {
    CU_ASSERT_TRUE(pm_get_page_status(heap, i));
  }
  h_delete(heap);
}

void test_pagemap_set_all_pages_to_active_then_to_passive() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);
  int no_pages = h_page_amount(heap);
  
  for (int i = 0; i < no_pages; i++) {
    pm_set_page_to(heap, i, true);
    CU_ASSERT_TRUE(pm_get_page_status(heap, i));
  }
  for (int i = 0; i < no_pages; i++) {
    CU_ASSERT_TRUE(pm_get_page_status(heap, i));
  }
  for (int i = 0; i < no_pages; i++) {
    pm_set_page_to(heap, i, false);
    CU_ASSERT_FALSE(pm_get_page_status(heap, i));
  }
  for (int i = 0; i < no_pages; i++) {
    CU_ASSERT_FALSE(pm_get_page_status(heap, i));
  }

  h_delete(heap);
}

void test_pagemap_set_arbitrary_page_to_active() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);
  
  int page_index = 42;
  pm_set_page_to(heap, page_index, true);
  CU_ASSERT_FALSE(pm_get_page_status(heap, page_index - 1));
  CU_ASSERT_TRUE(pm_get_page_status(heap, page_index));
  CU_ASSERT_FALSE(pm_get_page_status(heap, page_index + 1));
  h_delete(heap);
}

void test_pagemap_set_arbitrary_page_to_passive() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);
  int no_pages = h_page_amount(heap);
  
  for (int i = 0; i < no_pages; i++) {
    pm_set_page_to(heap, i, true);
    CU_ASSERT_TRUE(pm_get_page_status(heap, i));
  }
  for (int i = 0; i < no_pages; i++) {
    CU_ASSERT_TRUE(pm_get_page_status(heap, i));
  }

  int page_index = 23;
  pm_set_page_to(heap, page_index, false);
  CU_ASSERT_TRUE(pm_get_page_status(heap, page_index - 1));
  CU_ASSERT_FALSE(pm_get_page_status(heap, page_index));
  CU_ASSERT_TRUE(pm_get_page_status(heap, page_index + 1));

  h_delete(heap);
}

// ALLOCATION MAP TESTS (artificial allocations)

void test_allocation_maps_initialised_to_zeroes() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);
  int no_pages = h_page_amount(heap);

  for (int i = 0; i < no_pages; i++) {
    page_t *page = h_get_page(heap, i);
    bitvector_t *allocation_map = p_get_alloc_map(page);
    
    for (int i = 0; i < BITVECTOR_SIZE * 2; i++) {
      CU_ASSERT_FALSE(bitvector_array_read_bit(allocation_map, i));
    }
  }
  h_delete(heap);
}

void set_all_blocks_to_allocated(heap_t *heap) {
  int no_pages = h_page_amount(heap);

  for (int i = 0; i < no_pages; i++) {
    page_t *page = h_get_page(heap, i);
    bitvector_t *allocation_map = p_get_alloc_map(page);
    
    for (int i = 0; i < BITVECTOR_SIZE * 2; i++) {
      am_set_allocation(heap, am_index_to_ptr(page, i), true);
      CU_ASSERT_TRUE(bitvector_array_read_bit(allocation_map, i));
    }
  }
  for (int i = 0; i < no_pages; i++) {
    page_t *page = h_get_page(heap, i);
    bitvector_t *allocation_map = p_get_alloc_map(page);
    
    for (int i = 0; i < BITVECTOR_SIZE * 2; i++) {
      CU_ASSERT_TRUE(bitvector_array_read_bit(allocation_map, i));
    }
  }
}

void test_am_set_all_blocks_to_allocated() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  set_all_blocks_to_allocated(heap);
  h_delete(heap);
}

void test_allocate_arbitrary_block() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);

  int page_index = 23;
  page_t *page = h_get_page(heap, page_index);
  bitvector_t *allocation_map = p_get_alloc_map(page);
  int start_index = 42;

  am_set_allocation(heap, am_index_to_ptr(page, start_index), true);
  CU_ASSERT_FALSE(bitvector_array_read_bit(allocation_map, start_index - 1));
  CU_ASSERT_FALSE(am_lookup(heap, am_index_to_ptr(page, start_index - 1)));
  CU_ASSERT_TRUE(bitvector_array_read_bit(allocation_map, start_index));
  CU_ASSERT_TRUE(am_lookup(heap, am_index_to_ptr(page, start_index)));
  CU_ASSERT_FALSE(bitvector_array_read_bit(allocation_map, start_index + 1));
  CU_ASSERT_FALSE(am_lookup(heap, am_index_to_ptr(page, start_index + 1)));
  h_delete(heap);
}


void test_allocate_several_blocks() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);

  int page_index = 23;
  page_t *page = h_get_page(heap, page_index);
  bitvector_t *allocation_map = p_get_alloc_map(page);
  int start_index = 42;
  int no_blocks = 3;
  
  for (int i = 0; i < no_blocks; i++)
  {
    am_set_allocation(heap, am_index_to_ptr(page, start_index + i), true);
  }
  
  CU_ASSERT_FALSE(bitvector_array_read_bit(allocation_map, start_index - 1));
  CU_ASSERT_FALSE(am_lookup(heap, am_index_to_ptr(page, start_index - 1)));

  for (int i = start_index; i < start_index + no_blocks; i++) {
    CU_ASSERT_TRUE(bitvector_array_read_bit(allocation_map, i));
    CU_ASSERT_TRUE(am_lookup(heap, am_index_to_ptr(page, i)));
  }
 
  CU_ASSERT_FALSE(bitvector_array_read_bit(allocation_map, start_index + no_blocks));
  CU_ASSERT_FALSE(am_lookup(heap, am_index_to_ptr(page, start_index + no_blocks)));
  h_delete(heap);
}

void test_clear_allocation_map() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  set_all_blocks_to_allocated(heap);
  int no_pages = h_page_amount(heap);
  
  for (int i = 0; i < no_pages; i++) {
    page_t *page = h_get_page(heap, i);
    am_clear(page);
  }
  for (int i = 0; i < no_pages; i++) {
    CU_ASSERT_FALSE(pm_get_page_status(heap, i));
  }
  h_delete(heap);
}

// ALLOCATION MAP TESTS (real allocations)

void test_allocate_small_object_integrated() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  void *allocated_ptr = h_alloc_data(heap, 8);
  CU_ASSERT_TRUE(am_lookup(heap, allocated_ptr));
  CU_ASSERT_TRUE(am_lookup(heap, ((char *)allocated_ptr - 8)));
  CU_ASSERT_FALSE(am_lookup(heap, ((char *)allocated_ptr + 8)));
  h_delete(heap);
}



// void test_allocate_large_object_integrated() {
//   heap_t *heap = h_init(PAGES_192, true, 1.0);
//   void *allocated_ptr = h_alloc_data(heap, 286);
//   CU_ASSERT_TRUE(am_lookup(heap, allocated_ptr));
//   CU_ASSERT_TRUE(am_lookup(heap, ((char *)allocated_ptr - 8)));
  
//   page_t *page = h_get_page(heap, 0);
//   bitvector_t *allocation_map = p_get_alloc_map(page);
    
//   for (int i = 1; i < BITVECTOR_SIZE * 2; i++) {
//     CU_ASSERT_FALSE(bitvector_array_read_bit(allocation_map, i));
//   }

//   int no_pages = h_page_amount(heap);

//   for (int i = 1; i < no_pages; i++) {
//     page_t *page = h_get_page(heap, i);
//     bitvector_t *allocation_map = p_get_alloc_map(page);
    
//     for (int i = 0; i < BITVECTOR_SIZE * 2; i++) {
//       CU_ASSERT_FALSE(bitvector_array_read_bit(allocation_map, i));
//     }
//   }
//   h_delete(heap);
// }




// PRESSURE CHECK TESTS

void set_number_of_pages_to_active(heap_t *heap, int no_pages_to_activate) {
 for (int i = 0; i < no_pages_to_activate; i++) {
    pm_set_page_to(heap, i, true);
  }
}

void test_pressure_full_heap(size_t heap_size_in_bytes) {
  heap_t *heap = h_init(heap_size_in_bytes, true, 1.0);
  int no_pages = h_page_amount(heap);
  int no_pages_to_activate = no_pages / 2;
  set_number_of_pages_to_active(heap, no_pages_to_activate);
  CU_ASSERT_EQUAL(pm_get_memory_pressure(heap), 1.0);
  h_delete(heap);
}

void test_pressure_even_no_pages_full() {
  test_pressure_full_heap(PAGES_192);
}

void test_pressure_odd_no_pages_full() {
  test_pressure_full_heap(PAGES_191);
}

void test_pressure_empty_heap(size_t heap_size_in_bytes) {
  heap_t *heap = h_init(heap_size_in_bytes, true, 1.0);
  CU_ASSERT_EQUAL(pm_get_memory_pressure(heap), 0);
  h_delete(heap);
}

void test_pressure_even_no_pages_empty() {
  test_pressure_empty_heap(PAGES_192);
}

void test_pressure_odd_no_pages_empty() {
  test_pressure_empty_heap(PAGES_191);
}

void test_pressure_arbitrary_no_pages_active(size_t heap_size_in_bytes, float expected_pressure) {
  heap_t *heap = h_init(heap_size_in_bytes, true, 1.0);
  int no_pages_to_activate = 42;
  set_number_of_pages_to_active(heap, no_pages_to_activate);
  CU_ASSERT_EQUAL(pm_get_memory_pressure(heap), expected_pressure);
  h_delete(heap);
}

void test_pressure_even_no_pages_arbitrary_no_active() {
  test_pressure_arbitrary_no_pages_active(PAGES_192, 0.4375);
}

void test_pressure_odd_no_pages_arbitrary_no_active() {
  test_pressure_arbitrary_no_pages_active(PAGES_191, ((float)42) / 95);
}

 
int main()
{
  CU_pSuite allocation_map_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  allocation_map_test_suite = CU_add_suite("Testing allocation_map and page_map", init_suite, clean_suite);
  if (NULL == allocation_map_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      //(NULL == CU_add_test(allocation_map_test_suite, "Free space on unused page is PAGESIZE (2048)", check_page_size)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test that the pagemap is initialised to all zeroes", test_pagemap_initialisation)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test setting all pages in pagemap to active", test_pagemap_set_all_pages_to_active)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test setting all pages in pagemap to active then back to passive", test_pagemap_set_all_pages_to_active_then_to_passive)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test setting an arbitrary page in pagemap to active", test_pagemap_set_arbitrary_page_to_active)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test setting an arbitrary page in pagemap to passive", test_pagemap_set_arbitrary_page_to_passive)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test that all allocation maps are initialised to zeroes", test_allocation_maps_initialised_to_zeroes)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test setting all blocks to allocated in allocation maps", test_am_set_all_blocks_to_allocated)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test allocating a single arbitrary block", test_allocate_arbitrary_block)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test allocating a several consecutive blocks", test_allocate_several_blocks)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test clearing all the allocation maps in a heap", test_clear_allocation_map)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test full memory pressure with an even number of pages in heap", test_pressure_even_no_pages_full)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test full memory pressure with an odd number of pages in heap", test_pressure_odd_no_pages_full)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test memory pressure on empty heap with an even number of pages in heap", test_pressure_even_no_pages_empty)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test memory pressure on empty heap with an odd number of pages in heap", test_pressure_odd_no_pages_empty)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test an arbitrary memory pressure on a heap with an even number of pages", test_pressure_even_no_pages_arbitrary_no_active)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test an arbitrary memory pressure on a heap with an odd number of pages", test_pressure_odd_no_pages_arbitrary_no_active)) ||
      (NULL == CU_add_test(allocation_map_test_suite, "Test allocating a small object using `h_alloc_data`", test_allocate_small_object_integrated)) ||
      // (NULL == CU_add_test(allocation_map_test_suite, "Test allocating a large object using `h_alloc_data`", test_allocate_large_object_integrated)) ||
      0
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
