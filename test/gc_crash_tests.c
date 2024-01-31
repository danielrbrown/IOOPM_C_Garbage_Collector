#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "../src/gc.h"
#include "../src/page/page.h" //P_ALLOC_SIZE
int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}

#define HEAP_SIZE_GC_CRASH_TEST P_ALLOC_SIZE * 64 + h_metadata_size() + sizeof(bitvector_t)
#define ITERATION_AMOUNT 10000
void fill_heap_to_trigger_garbage_collection_multiple_times(void)
{
  heap_t *heap = h_init(HEAP_SIZE_GC_CRASH_TEST, false, 0.5);
  for(int i = 0; i < ITERATION_AMOUNT; i++) {
      h_alloc_struct(heap, "**i");
  }
  
}


int main()
{
  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Test Suite 1", init_suite, clean_suite);
  if (NULL == test_suite1)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(test_suite1, "Fill heap to trigger garbage collection mutliple times", fill_heap_to_trigger_garbage_collection_multiple_times))  
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
