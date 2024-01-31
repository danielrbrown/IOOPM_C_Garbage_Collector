#include "../src/gc.h"
#include "../src/page_map/page_map.h"
#include "../src/page/page.h"
#include "../src/allocation_map/allocation_map.h"
#include <stdbool.h>
#include <math.h>
#include <CUnit/Basic.h>

#define PAGES_192 (P_ALLOC_SIZE * 192 + h_metadata_size() + sizeof(bitvector_t) * 3) // Space for 192 pages and three full bitvectors in pagemap
#define PAGES_191 (P_ALLOC_SIZE * 191 + h_metadata_size() + sizeof(bitvector_t) * 3) // Space for 191 pages and three bitvector in pagemap

#define MIN_HEAP_SIZE P_ALLOC_SIZE * 4 + h_metadata_size() + sizeof(bitvector_t)

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

void test_heap_initialisations(size_t bytes, size_t expected_no_pages)
{
    heap_t *heap = h_init(bytes, true, 1.0);
    CU_ASSERT_PTR_NOT_NULL(heap);
    CU_ASSERT_EQUAL(h_page_amount(heap), expected_no_pages);
    long calculated = (h_page_amount(heap) * P_ALLOC_SIZE);
    calculated += h_metadata_size() + 8 * (ceil((float)h_page_amount(heap) / 64));
    CU_ASSERT_EQUAL(h_total_bytes(heap), calculated);
    h_delete(heap);
}

void test_init_delete_heap()
{
    void *heap = NULL;
    heap = h_init(5000, true, 1.0);
    CU_ASSERT_PTR_NOT_NULL(heap);
    h_delete(heap);
}

void test_heap_correct_number_of_pages()
{
    heap_t *heap = h_init(16000, true, 1.0);
    CU_ASSERT_EQUAL(h_page_amount(heap), 7);
    h_delete(heap);
}

void test_heap_correct_size()
{
    test_heap_initialisations(9000, 4);
}

void test_smallest_heap_possible()
{
  heap_t *heap = h_init(h_metadata_size() + sizeof(bitvector_t) + P_ALLOC_SIZE * 2, true, 1.0);
  CU_ASSERT_PTR_NOT_NULL(heap);
  CU_ASSERT_EQUAL(h_page_amount(heap), 2);
  h_delete(heap);
}

void test_100million_bytes() // Allocating 100 MB
{
  test_heap_initialisations(100000000, 48074);
}

void test_unsafestack_gcthreshold()
{
    heap_t *heap1 = h_init(6000, true, 1.0);
    CU_ASSERT_PTR_NOT_NULL(heap1);
    CU_ASSERT_EQUAL(h_unsafe_stack(heap1), true);
    CU_ASSERT_EQUAL(h_gc_threshold(heap1), 1.0);
    h_delete(heap1);

    heap_t *heap2 = h_init(6000, false, 0.5);
    CU_ASSERT_PTR_NOT_NULL(heap2);
    CU_ASSERT_EQUAL(h_unsafe_stack(heap2), false);
    CU_ASSERT_EQUAL(h_gc_threshold(heap2), 0.5);
    h_delete(heap2);
}

void test_first_page_address()
{
    heap_t *heap = h_init(7000, true, 1.0);
    long h_address = (long)heap;
    long p_address = (long)h_first_page(heap);
    int pagemap_size = 8 * (ceil((float)h_page_amount(heap) / 64));
    CU_ASSERT_EQUAL(p_address - h_address, h_metadata_size() + pagemap_size);

    h_delete(heap);
}

void test_first_page_accessible()
{
    heap_t *heap = h_init(7000, true, 1.0);
    page_t *p_address = h_first_page(heap);
    void *p_first_free = p_next_free(p_address);
    CU_ASSERT_EQUAL((char *)p_first_free - (char *)p_address, p_metadata_size());

    h_delete(heap);
}

void test_second_page_accessible()
{
    heap_t *heap = h_init(7000, false, 0);
    page_t *p2_address = (page_t *)((char *)h_first_page(heap) + P_ALLOC_SIZE);
    void *p2_first_free = p_next_free(p2_address);
    CU_ASSERT_EQUAL((char *)p2_first_free - (char *)p2_address, p_metadata_size());

    h_delete(heap);
}

void test_calculate_no_pages_1() {
  
  // Space for exactly 192 pages and 3 full bitvectors in pagemap
  test_heap_initialisations(PAGES_192, 192);
}

void test_calculate_no_pages_2() {
  
  // Insufficient space for 192 pages and 3 full bitvectors in pagemap
  test_heap_initialisations(PAGES_192 - 1, 191);
}

void test_calculate_no_pages_3() {
  
  // Sufficient space for 192 pages and 3 full bitvectors in pagemap with surplus
  test_heap_initialisations(PAGES_192 + 1, 192);
}

void test_calculate_no_pages_4() {
  
  // Sufficient space for 193 pages but not an extra bitvector 
  test_heap_initialisations(PAGES_192 + P_ALLOC_SIZE, 192);
}

void test_calculate_no_pages_5() {
  
  // Sufficient space for 193 pages and an extra bitvector 
  test_heap_initialisations(PAGES_192 + P_ALLOC_SIZE + sizeof(bitvector_t), 193);
}

void test_calculate_no_pages_6() {
  
  // Sufficient space for 193 pages and an extra bitvector with surplus 
  test_heap_initialisations(PAGES_192 + P_ALLOC_SIZE + sizeof(bitvector_t) + 1, 193);
}

void test_get_page()
{
    heap_t *heap = h_init(20776, true, 1.0);
    CU_ASSERT_PTR_NOT_NULL(heap);
    page_t *third_page = h_get_page(heap, 2);
    page_t *third_page_calculated = (page_t *)(((char *)h_first_page(heap)) + (P_ALLOC_SIZE * 2));
    CU_ASSERT_PTR_EQUAL(third_page, third_page_calculated);
    page_t *fifth_page = h_get_page(heap, 4);
    page_t *fifth_page_calculated = (page_t *)(((char *)h_first_page(heap)) + (P_ALLOC_SIZE * 4));
    CU_ASSERT_PTR_EQUAL(fifth_page, fifth_page_calculated);
    page_t *first_page = h_get_page(heap, 0);
    CU_ASSERT_PTR_EQUAL(first_page, h_first_page(heap));
    h_delete(heap);
}

void test_available_memory_empty_heap()
{
    heap_t *heap = h_init(19000, true, 1.0);
    int expected_available = h_page_amount(heap) * PAGESIZE;
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);
    h_delete(heap);
}

static size_t calculated_alloc_size(size_t bytes)
{
    if (bytes <= 0)
    {
        return 0;
    }
    else if (bytes < WORD_SIZE)
    {
        return MIN_ALLOC_SIZE;
    }
    else
    {
        assert(bytes <= MAX_ALLOC_SIZE && "Error: Allocation too large");
        int blocks = 0;
        if (bytes % WORD_SIZE == 0)
        {
            blocks = (bytes / WORD_SIZE);
        }
        else
        {
            blocks = 1 + (bytes / WORD_SIZE);
        }
        return (size_t)(blocks * WORD_SIZE + sizeof(header_t));
    }
}

void test_available_memory_after_one_raw_alloc()
{
    heap_t *heap = h_init(6000, true, 1.0);
    int expected_available = (h_page_amount(heap) * PAGESIZE);
    char *some_alloc = h_alloc_data(heap, 80);
    expected_available -= calculated_alloc_size(80);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);
    some_alloc++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_available_memory_after_multiple_raw_allocs()
{
    heap_t *heap = h_init(12000, true, 1.0);
    int expected_available = (h_page_amount(heap) * PAGESIZE);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    size_t allocsize = 80;
    int *alloc1 = h_alloc_data(heap, allocsize);
    expected_available -= calculated_alloc_size(allocsize);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    allocsize = 40;
    int *alloc2 = h_alloc_data(heap, allocsize);
    expected_available -= calculated_alloc_size(allocsize);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    allocsize = 8;
    int *alloc3 = h_alloc_data(heap, allocsize);
    expected_available -= calculated_alloc_size(allocsize);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    alloc1++; // to get rid of gcc warning about unused variable
    alloc2++; // to get rid of gcc warning about unused variable
    alloc3++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_available_memory_after_one_struct_alloc()
{
    heap_t *heap = h_init(8000, true, 1.0);
    int expected_available = (h_page_amount(heap) * PAGESIZE);
    char *layout = "***";
    char *some_alloc = h_alloc_struct(heap, layout);
    expected_available -= (3 * sizeof(void *) + sizeof(header_t));
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);
    some_alloc++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_available_memory_after_multiple_struct_allocs()
{
    heap_t *heap = h_init(15000, true, 1.0);
    int expected_available = (h_page_amount(heap) * PAGESIZE);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *layout1 = "70*";
    char *alloc1 = h_alloc_struct(heap, layout1);
    // to account for the format string that gets allocated we need to add 2 * sizeof(header_t)
    expected_available -= calculated_alloc_size(70 * sizeof(void *) + 2 * sizeof(header_t));
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *layout2 = "*i**";
    char *alloc2 = h_alloc_struct(heap, layout2);
    expected_available -= calculated_alloc_size(3 * sizeof(void *) + sizeof(int));
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *layout3 = "fiffii";
    char *alloc3 = h_alloc_struct(heap, layout3);
    expected_available -= calculated_alloc_size(3 * sizeof(float) + 3 * sizeof(int));
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *layout4 = "i*i*i*f";
    char *alloc4 = h_alloc_struct(heap, layout4);
    // since pointers are larger than int or float we need to account for largest field size
    expected_available -= calculated_alloc_size(3 * sizeof(void *) + 4 * sizeof(void *));
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    alloc1++; // to get rid of gcc warning about unused variable
    alloc2++; // to get rid of gcc warning about unused variable
    alloc3++; // to get rid of gcc warning about unused variable
    alloc4++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_available_memory_after_various_allocs()
{
    heap_t *heap = h_init(5000, true, 1.0);
    int expected_available = (h_page_amount(heap) * PAGESIZE);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *alloc1 = h_alloc_data(heap, 99);
    expected_available -= calculated_alloc_size(99);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *layout1 = "cildf";
    char *alloc2 = h_alloc_struct(heap, layout1);
    expected_available -= calculated_alloc_size(sizeof(char) + sizeof(int) + sizeof(long) + sizeof(double) + sizeof(float));
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *alloc3 = h_alloc_data(heap, 200);
    expected_available -= calculated_alloc_size(200);
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    char *layout2 = "136*14d";
    char *alloc4 = h_alloc_struct(heap, layout2);
    // to account for the format string that gets allocated we need to add 2 * sizeof(header_t)
    expected_available -= calculated_alloc_size(136 * sizeof(void *) + 14 * sizeof(double) + 2 * sizeof(header_t));
    CU_ASSERT_EQUAL(h_avail(heap), expected_available);

    alloc1++; // to get rid of gcc warning about unused variable
    alloc2++; // to get rid of gcc warning about unused variable
    alloc3++; // to get rid of gcc warning about unused variable
    alloc4++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_used_memory_empty_heap()
{
    heap_t *heap = h_init(15000, true, 1.0);
    int expected_used = 0;
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    h_delete(heap);
}

void test_used_memory_one_raw_alloc()
{
    heap_t *heap = h_init(11000, true, 1.0);
    int expected_used = 0;
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    size_t allocsize = 1234;
    int *alloc = h_alloc_data(heap, allocsize);
    expected_used += calculated_alloc_size(allocsize) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    alloc++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_used_memory_multiple_raw_allocs()
{
    heap_t *heap = h_init(13000, true, 1.0);
    int expected_used = 0;
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    size_t allocsize = 1111;
    int *alloc1 = h_alloc_data(heap, allocsize);
    expected_used += calculated_alloc_size(allocsize) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    allocsize = 1;
    int *alloc2 = h_alloc_data(heap, allocsize);
    expected_used += calculated_alloc_size(allocsize) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    allocsize = 11;
    int *alloc3 = h_alloc_data(heap, allocsize);
    expected_used += calculated_alloc_size(allocsize) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    alloc1++; // to get rid of gcc warning about unused variable
    alloc2++; // to get rid of gcc warning about unused variable
    alloc3++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_used_memory_one_struct_alloc()
{
    heap_t *heap = h_init(21000, true, 1.0);
    int expected_used = 0;
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    char *layout = "i*i";
    char *some_alloc = h_alloc_struct(heap, layout);
    expected_used += (3 * sizeof(void *));

    some_alloc++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_used_memory_multiple_struct_allocs()
{
    heap_t *heap = h_init(43000, true, 1.0);
    size_t expected_used = 0;
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    char *layout1 = "i*i";
    char *alloc1 = h_alloc_struct(heap, layout1);
    expected_used += calculated_alloc_size(3 * sizeof(void *)) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    char *layout2 = "15d3f";
    char *alloc2 = h_alloc_struct(heap, layout2);
    expected_used += calculated_alloc_size(15 * sizeof(double) + 3 * sizeof(float)) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    char *layout3 = "67";
    char *alloc3 = h_alloc_struct(heap, layout3);
    expected_used += calculated_alloc_size(67 * sizeof(char)) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    char *layout4 = "100*";
    char *alloc4 = h_alloc_struct(heap, layout4);
    expected_used += calculated_alloc_size(100 * sizeof(void *)) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    alloc1++; // to get rid of gcc warning about unused variable
    alloc2++; // to get rid of gcc warning about unused variable
    alloc3++; // to get rid of gcc warning about unused variable
    alloc4++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_used_memory_various_allocs()
{
    heap_t *heap = h_init(30000, true, 1.0);
    int expected_used = 0;
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    // max size alloc
    char *layout1 = "254*1l";
    char *alloc1 = h_alloc_struct(heap, layout1);
    expected_used += calculated_alloc_size(254 * sizeof(void *) + sizeof(long)) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    size_t allocsize = 1;
    char *alloc2 = h_alloc_data(heap, allocsize);
    expected_used += calculated_alloc_size(allocsize) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    char *layout2 = "2f5d7i3c";
    char *alloc3 = h_alloc_struct(heap, layout2);
    expected_used += calculated_alloc_size(2 * sizeof(float) + 5 * sizeof(double) + 7 * sizeof(int) + 3 * sizeof(char)) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    allocsize = 222;
    char *alloc4 = h_alloc_data(heap, allocsize);
    expected_used += calculated_alloc_size(allocsize) - sizeof(header_t);
    CU_ASSERT_EQUAL(h_used(heap), expected_used);

    alloc1++; // to get rid of gcc warning about unused variable
    alloc2++; // to get rid of gcc warning about unused variable
    alloc3++; // to get rid of gcc warning about unused variable
    alloc4++; // to get rid of gcc warning about unused variable
    h_delete(heap);
}

void test_exceed_threshhold_and_garbage_collect(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.5);
    size_t mem_before_gc = h_avail(heap);
    void *object = NULL;
    void *object2 = h_alloc_struct(heap, "15*");
    void *tmp = object2;
    for(int i = 0; i < 16; i++){
        object = h_alloc_struct(heap, "15*");
    }
    size_t mem_after_gc = h_avail(heap);
    CU_ASSERT_TRUE(mem_after_gc == mem_before_gc - 3 * (sizeof(void *) * 15 + sizeof(header_t)));

    h_delete(heap);
}

void test_garbage_collect_and_copied_correct(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.5);
    void *object = h_alloc_struct(heap, "15*");
    void *tmp = object;
    h_gc(heap);
    CU_ASSERT_TRUE(pm_index_from_pointer(heap, object) == 1);
    h_delete(heap);
}

void test_manual_garbage_collect(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.5);
    void *object = NULL;
    size_t mem_before_gc = h_avail(heap);
    for(int i = 0; i < 16; i++){ //Make first page completely full
        object = h_alloc_struct(heap, "15*");
    }
    h_gc(heap);
    size_t mem_after_gc = h_avail(heap);
    CU_ASSERT_EQUAL(mem_before_gc, mem_after_gc + 128); //Each allocated object is 128 bytes
                                                             //the last object still has a pointer on the stack
    h_delete(heap);
}

int main()
{
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite heap_tests = CU_add_suite("Testing heap functionality", init_suite, clean_suite);
    if (heap_tests == NULL)
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
        (CU_add_test(heap_tests, "Creating and deleting a heap", test_init_delete_heap) == NULL) ||
        (CU_add_test(heap_tests, "Heap created with only 1 page", test_heap_correct_number_of_pages) == NULL) ||
        (CU_add_test(heap_tests, "Checking if heap size correct", test_heap_correct_size) == NULL) ||
        (CU_add_test(heap_tests, "Checking smallest heap possible (4200 bytes)", test_smallest_heap_possible) == NULL) ||
        (CU_add_test(heap_tests, "Testing a heap with 100 million bytes", test_100million_bytes) == NULL) ||
        (CU_add_test(heap_tests, "Testing gc_threshold and unsafestack (irrelevant right now)", test_unsafestack_gcthreshold) == NULL) ||
        (CU_add_test(heap_tests, "Check first page has expected address", test_first_page_address) == NULL) ||
        (CU_add_test(heap_tests, "Check first page can be accessed", test_first_page_accessible) == NULL) ||
        (CU_add_test(heap_tests, "Check second page can be accessed", test_second_page_accessible) == NULL) ||
        (CU_add_test(heap_tests, "Check that the correct number of pages are created 1", test_calculate_no_pages_1) == NULL) ||
        (CU_add_test(heap_tests, "Check that the correct number of pages are created 2", test_calculate_no_pages_2) == NULL) ||
        (CU_add_test(heap_tests, "Check that the correct number of pages are created 3", test_calculate_no_pages_3) == NULL) ||
        (CU_add_test(heap_tests, "Check that the correct number of pages are created 4", test_calculate_no_pages_4) == NULL) ||
        (CU_add_test(heap_tests, "Check that the correct number of pages are created 5", test_calculate_no_pages_5) == NULL) ||
        (CU_add_test(heap_tests, "Check that the correct number of pages are created 6", test_calculate_no_pages_6) == NULL) ||
        (CU_add_test(heap_tests, "Test returning a page of a given index", test_get_page) == NULL) ||
        (CU_add_test(heap_tests, "Check empty heap has expected amount of available memory", test_available_memory_empty_heap) == NULL) ||
        (CU_add_test(heap_tests, "Heap has expected amount of available memory after one raw alloc", test_available_memory_after_one_raw_alloc) == NULL) ||
        (CU_add_test(heap_tests, "Heap has expected amount of available memory after multiple raw allocs", test_available_memory_after_multiple_raw_allocs) == NULL) ||
        (CU_add_test(heap_tests, "Heap has expected amount of available memory after one struct alloc", test_available_memory_after_one_struct_alloc) == NULL) ||
        (CU_add_test(heap_tests, "Heap has expected amount of available memory after multiple struct allocs", test_available_memory_after_multiple_struct_allocs) == NULL) ||
        (CU_add_test(heap_tests, "Heap has expected amount of available memory after various mixed allocs", test_available_memory_after_various_allocs) == NULL) ||
        (CU_add_test(heap_tests, "h_used returns 0 for freshly initialized heap", test_used_memory_empty_heap) == NULL) ||
        (CU_add_test(heap_tests, "h_used returns correct amount after one raw alloc", test_used_memory_one_raw_alloc) == NULL) ||
        (CU_add_test(heap_tests, "h_used returns correct amount after multiple raw allocs", test_used_memory_multiple_raw_allocs) == NULL) ||
        (CU_add_test(heap_tests, "h_used returns correct amount after one struct alloc", test_used_memory_one_struct_alloc) == NULL) ||
        (CU_add_test(heap_tests, "h_used returns correct amount after multiple struct allocs", test_used_memory_multiple_struct_allocs) == NULL) ||
        (CU_add_test(heap_tests, "h_used returns correct amount after various mixed allocs", test_used_memory_various_allocs) == NULL) ||
        (CU_add_test(heap_tests, "Check that garbage collection is called when allocation on new page", test_exceed_threshhold_and_garbage_collect) == NULL) ||
        (CU_add_test(heap_tests, "Check that objects are copied correctly after garbage collection", test_garbage_collect_and_copied_correct) == NULL) ||
        (CU_add_test(heap_tests, "Check that manual garbage collection works", test_manual_garbage_collect) == NULL) ||
        0)
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