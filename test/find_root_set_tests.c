#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../src/allocation_map/allocation_map.h"
#include "../src/find_root_set/find_root_set.h"
#include "../src/header/header.h"
#include "../src/page_map/page_map.h"

#define MIN_HEAP_SIZE P_ALLOC_SIZE * 4 + h_metadata_size() + sizeof(bitvector_t)
#define MIN_ALLOC_SIZE 16

struct test_struct {
    void *ptr1;
    void *ptr2;
    void *ptr3;
    int num;
};

typedef struct test_struct test_struct_t;
typedef struct tree tree_t;

struct tree {
    tree_t *lhs;
    tree_t *rhs;
    int num;
};



/**
 * @brief Create a tree object with two branches
 * 
 * @param heap 
 * @return a tree_t* that looks like this:
 *         tree
 *          ├─ lhs
 *          │   ├─ NULL
 *          │   ├─ NULL
 *          │   ├─ 50
 *          ├─ rhs
 *          │   ├─ NULL
 *          │   ├─ NULL
 *          │   ├─ 50
 *          ├─ 50
 *
 */
tree_t *_create_tree(heap_t *heap) {
    tree_t *tree = h_alloc_struct(heap, "**i");
    
    tree->num = 50;
    tree->lhs = NULL;
    tree->rhs = NULL;
    return tree;
}

tree_t *create_tree(heap_t *heap) {
    tree_t *tree = _create_tree(heap);
    tree->lhs = _create_tree(heap);
    tree->rhs = _create_tree(heap);
    return tree;
}

static void test_empty_heap() {
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.8);

    size_t mem = h_avail(heap);

    find_root_set(heap);

    CU_ASSERT_EQUAL(mem, h_avail(heap));

    h_delete(heap);
}

static void test_find_root_set_zero_collected(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, true, 0.8);

    size_t all_mem = h_avail(heap);

    void *object = h_alloc_struct(heap, "i");
    header_t *header = (header_t *) (((char *) object) - sizeof(header_t));
    size_t size_of_object_alloc = size_of_raw_allocation(*header) + sizeof(header_t);

    CU_ASSERT_TRUE(am_lookup(heap, object));
    size_t mem = h_avail(heap);
    
    CU_ASSERT_TRUE(am_lookup(heap, object));    
    CU_ASSERT_EQUAL(mem + 1 * size_of_object_alloc , all_mem);

    find_root_set(heap);

    size_t mem_after = h_avail(heap);
    CU_ASSERT_EQUAL(mem_after + 1 * size_of_object_alloc , all_mem);
    CU_ASSERT_TRUE(am_lookup(heap, object));

    h_delete(heap);
}

static void test_find_root_set_collect(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.8);

    size_t all_mem = h_avail(heap);

    void *object = h_alloc_struct(heap, "i");
    void *object2 = h_alloc_struct(heap, "i");

    header_t *header = (header_t *) (((char *) object) - sizeof(header_t));
    size_t size_of_object_alloc = size_of_raw_allocation(*header) + sizeof(header_t);

    CU_ASSERT_TRUE(am_lookup(heap, object));
    size_t mem = h_avail(heap);

    CU_ASSERT_EQUAL(mem + 2 * size_of_object_alloc, all_mem);

    find_root_set(heap);
    size_t mem_after = h_avail(heap);

    CU_ASSERT_TRUE(am_lookup(heap, object));
    CU_ASSERT_EQUAL(mem_after + 4 * size_of_object_alloc, all_mem);

    h_delete(heap);
}

static void different_function(heap_t *heap, size_t all_mem) 
{
    void *object2 = h_alloc_struct(heap, "i");

    header_t *header = (header_t *) (((char *) object2) - sizeof(header_t));
    size_t size_of_object_alloc = size_of_raw_allocation(*header) + sizeof(header_t);

    CU_ASSERT_TRUE(am_lookup(heap, object2));
    size_t mem = h_avail(heap);

    CU_ASSERT_EQUAL(mem + 2 * size_of_object_alloc, all_mem);

    find_root_set(heap);

    size_t mem_after = h_avail(heap);

    CU_ASSERT_EQUAL(mem_after + 4 * size_of_object_alloc, all_mem);

}

static void test_find_root_set_objects_in_different_functions(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.8);

    size_t all_mem = h_avail(heap);

    void *object = h_alloc_struct(heap, "i");
    CU_ASSERT_TRUE(am_lookup(heap, object));

    different_function(heap, all_mem);
    
    h_delete(heap);
}

static void test_find_root_set_tree(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.8);
    size_t all_mem = h_avail(heap);

    tree_t *tree = create_tree(heap);
    size_t mem = h_avail(heap);
    size_t size_of_tree = all_mem - mem;
    CU_ASSERT_TRUE(am_lookup(heap, tree));

    find_root_set(heap);

    mem = h_avail(heap);

    CU_ASSERT_TRUE(am_lookup(heap, tree));
    CU_ASSERT_EQUAL(mem + 2 * size_of_tree, all_mem);
    
    h_delete(heap);
}

static void test_kill_stack_pointers(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.8);
    size_t all_mem = h_avail(heap);

    tree_t *tree = create_tree(heap);
    size_t mem = h_avail(heap);
    size_t size_of_tree = all_mem - mem;

    void *object1 = h_alloc_struct(heap, "i");
    void *object2 = h_alloc_struct(heap, "i");
    header_t *header = (header_t *) (((char *) object1) - sizeof(header_t));
    size_t size_of_object_alloc = size_of_raw_allocation(*header) + sizeof(header_t);

    kill_stack_pointers(heap, NULL);

    mem = h_avail(heap);
    CU_ASSERT_PTR_NULL(tree);
    CU_ASSERT_PTR_NULL(object1);
    CU_ASSERT_PTR_NULL(object2);
    CU_ASSERT_EQUAL(mem + 1 * size_of_tree + 2 * size_of_object_alloc, all_mem);
    
    h_delete(heap);
}

static void test_kill_stack_pointers_empty_heap(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 0.8);
    size_t all_mem = h_avail(heap);

    kill_stack_pointers(heap, NULL);

    size_t mem = h_avail(heap);

    CU_ASSERT_PTR_NOT_NULL(heap);
    CU_ASSERT_EQUAL(mem, all_mem);
    
    h_delete(heap);
}


static int init_suite(void)
{
    // Change this function if you want to do something *before* you
    // run a test suite
    return 0;
}

static int clean_suite(void)
{
    // Change this function if you want to do something *after* you
    // run a test suite
    return 0;
}

int main()
{
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite my_test_suite = CU_add_suite("Find Root Set test suite\n", init_suite, clean_suite);
    if (my_test_suite == NULL)
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
    if ((CU_add_test(my_test_suite, "Test empty heap", test_empty_heap) == NULL) ||
        (CU_add_test(my_test_suite, "Test heap with no dead pointers", test_find_root_set_zero_collected) == NULL) ||
        (CU_add_test(my_test_suite, "Test heap with test struct", test_find_root_set_collect) == NULL) ||
        (CU_add_test(my_test_suite, "Test heap with layered functions", test_find_root_set_objects_in_different_functions) == NULL) ||
        (CU_add_test(my_test_suite, "Test heap with tree structure", test_find_root_set_tree) == NULL) ||
        (CU_add_test(my_test_suite, "Test kill stack pointers", test_kill_stack_pointers) == NULL) ||
        (CU_add_test(my_test_suite, "Test kill stack pointers empty heap", test_kill_stack_pointers_empty_heap) == NULL) || 0)
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