#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include "../src/garbage_collector/garbage_collector.h"
#include "../src/page/page.h"
#include "../src/page_map/page_map.h"

#define PAGES_192 (P_ALLOC_SIZE * 192 + h_metadata_size() + sizeof(bitvector_t) * 3) // Space for 192 pages and three full bitvectors in pagemap
#define PAGES_191 (P_ALLOC_SIZE * 191 + h_metadata_size() + sizeof(bitvector_t) * 3) // Space for 191 pages and three bitvector in pagemap
#define MIN_HEAP_SIZE P_ALLOC_SIZE * 4 + h_metadata_size() + sizeof(bitvector_t)

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

void check_heap_empty(heap_t *heap) {
  int no_pages = h_page_amount(heap);
  for (int i = 0; i < no_pages; i++)
  {
    page_t *page = h_get_page(heap, i);
    CU_ASSERT_FALSE(pm_get_page_status(heap, i));
    CU_ASSERT_TRUE(p_next_free(page) == p_start_address(page));
    CU_ASSERT_FALSE(p_contains_unsafe_objects(page));
  }
}

void test_gc_empty_heap() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  check_heap_empty(heap);
  garbage_collection(heap);
  check_heap_empty(heap);
  h_delete(heap);
}

void test_gc_with_unsafe_pages_heap() {
  heap_t *heap = h_init(PAGES_192, true, 1.0);
  for (int i = 0; i < 5; i++) // 5 = arbitrary number of pages with unsafe objects
  {
    page_t *page = h_get_page(heap, i);
    pm_set_page_to(heap, i, ACTIVE);
    p_set_unsafe_objects_flag(page, UNSAFE_OBJECTS);
  }
  garbage_collection(heap);
  for (int i = 0; i < 5; i++) // 5 = arbitrary number of pages with unsafe objects
  {
    page_t *page = h_get_page(heap, i);
    CU_ASSERT_TRUE(pm_get_page_status(heap, i));
    CU_ASSERT_FALSE(p_contains_unsafe_objects(page));
  }
  int no_pages = h_page_amount(heap);
  for (int i = 5; i < no_pages; i++) // 5 = arbitrary number of pages with unsafe objects
  {
    page_t *page = h_get_page(heap, i);
    CU_ASSERT_FALSE(pm_get_page_status(heap, i));
    CU_ASSERT_TRUE(p_next_free(page) == p_start_address(page));
    CU_ASSERT_FALSE(p_contains_unsafe_objects(page));
  }
  h_delete(heap);
}

int main()
{
  CU_pSuite garbage_collector_test_suite = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  garbage_collector_test_suite = CU_add_suite("Testing garbage_collector module", init_suite, clean_suite);
  if (NULL == garbage_collector_test_suite)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(garbage_collector_test_suite, "Test garbage collection on an empty heap", test_gc_empty_heap)) ||
      (NULL == CU_add_test(garbage_collector_test_suite, "Test garbage collection on a heap with several unsafe pages", test_gc_with_unsafe_pages_heap)) ||     
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
