#include "../src/traverse_and_copy/traverse_and_copy.h"
#include "../src/gc.h"
#include "../src/header/header.h"
#include "../src/allocation_map/allocation_map.h"
#include "../src/page/page.h"
#include "../src/page_map/page_map.h"
#include "../src/garbage_collector/garbage_collector.h"
#include <stdbool.h>
#include <CUnit/Basic.h>

typedef struct tree tree_t;
typedef struct pointer pointer_t;
typedef struct int_and_ptr int_and_ptr_t;
typedef struct multi_ptr multi_ptr_t;

#define MIN_HEAP_SIZE P_ALLOC_SIZE * 4 + h_metadata_size() + sizeof(bitvector_t)
#define MIN_ALLOC_SIZE 16

struct tree {
    tree_t *lhs;
    tree_t *rhs;
    int num;
};

struct pointer {
    void *ptr;
};

struct int_and_ptr {
    int num;
    void *ptr;
};

struct multi_ptr {
    int num1;
    void *ptr1;
    void *ptr3;
    int num2;
    void *ptr2;
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
 *          │   ├─ 100
 *          ├─ rhs
 *          │   ├─ NULL
 *          │   ├─ NULL
 *          │   ├─ 150
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


int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

static bool points_into_heap(heap_t * heap, const void *p)
{
    void *data_start = (void *) h_first_page(heap);
    void *data_end = (void *) ((char *) data_start + h_total_bytes(heap));
    return (p >= data_start && p <= data_end);
}

void test_traverse_and_copy_forwarded_object(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    void *object = h_alloc_struct(heap, "i");
    void *object_backup = object;
    CU_ASSERT_TRUE(am_lookup(heap, object));
    void *new_object = h_alloc_struct(heap, "i");
    header_t *old_header = (header_t *) (((char *) object) - sizeof(header_t));
    *old_header =
        create_header_with_forwarding_pointer(new_object);
    am_set_allocation(heap, object, false);
    CU_ASSERT_TRUE(is_forwarding_address_header(*old_header));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));
    CU_ASSERT_FALSE(am_lookup(heap, object));

    traverse_and_copy(heap, (void **) &object, object, true);
    
    CU_ASSERT_FALSE(am_lookup(heap, object_backup));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    h_delete(heap);
}

void test_traverse_and_copy_not_forwarded_object(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    void *object = h_alloc_struct(heap, "i");
    void *tmp = object;

    traverse_and_copy(heap, (void **) &object, object, true);
    
    CU_ASSERT_TRUE(points_into_heap(heap, object));
    CU_ASSERT_FALSE(object == tmp);
    page_t *page_copied_object = h_get_page(heap, pm_index_from_pointer(heap, object));
    page_t *page_original = h_get_page(heap, pm_index_from_pointer(heap, tmp));
    CU_ASSERT_TRUE(pm_page_is_active(heap, page_original));
    CU_ASSERT_FALSE(pm_page_is_active(heap, page_copied_object));
    h_delete(heap);
}

void test_traverse_and_copy_unforwarded_object_with_forwarded_child(){
    // NOTE: This test has an object with an "artificial" forwarding header.
    // The result of am_lookup for the object pointed to will therefore not
    // necessarily respresent the normal value, as GC should never be called 
    // with "reachable" objects with forwarding headers. The call to am_lookup for
    // the object pointed to has been adjusted accordingly.

    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    void **object = h_alloc_struct(heap, "*");
    void *child = h_alloc_struct(heap, "*");
    int *new_object = h_alloc_struct(heap, "i");
    void ** object_backup = object;
    void *child_backup = child;
    void *new_object_backup = new_object;
    *object = child;
    header_t *old_header = (header_t *) (((char *) child) - sizeof(header_t));
    *old_header = create_header_with_forwarding_pointer(new_object);
    am_set_allocation(heap, child, false);

    CU_ASSERT_TRUE(am_lookup(heap, object));
    CU_ASSERT_FALSE(am_lookup(heap, child));
    CU_ASSERT_TRUE(am_lookup(heap, new_object));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));
   
    traverse_and_copy(heap, (void **) &object, object, true);
    garbage_collection(heap);

    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));
    CU_ASSERT_FALSE(am_lookup(heap, object_backup));
    CU_ASSERT_FALSE(am_lookup(heap, child_backup));
    CU_ASSERT_FALSE(am_lookup(heap, new_object_backup));
    CU_ASSERT_TRUE(am_lookup(heap, object));
    h_delete(heap);
}

void test_traverse_and_copy_single_child() {
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    pointer_t *object = h_alloc_struct(heap, "*");
    int *child = h_alloc_struct(heap, "i");
    object->ptr = child; 
    pointer_t *object_backup = object;
    int *child_backup = child;
    page_t *page_object = h_get_page(heap, pm_index_from_pointer(heap, object));
    page_t *page_child = h_get_page(heap, pm_index_from_pointer(heap, (void *)child));

    CU_ASSERT_TRUE(am_lookup(heap, object));   
    CU_ASSERT_TRUE(am_lookup(heap, child));
    CU_ASSERT_TRUE(page_object == page_child);
    CU_ASSERT_PTR_EQUAL(page_object, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &object, object, true);

    CU_ASSERT_FALSE(am_lookup(heap, object_backup));
    CU_ASSERT_FALSE(am_lookup(heap, (void *)child_backup));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    garbage_collection(heap);

    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));
    h_delete(heap);
}

void test_traverse_and_copy_tree_struct(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    tree_t *tree = _create_tree(heap);
    void *tree_backup = tree;
    page_t *page_tree = h_get_page(heap, pm_index_from_pointer(heap, tree));
    
    CU_ASSERT_TRUE(am_lookup(heap, tree));
    CU_ASSERT_TRUE(am_lookup(heap, tree_backup));
    CU_ASSERT_PTR_EQUAL(page_tree, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &tree, tree, true);
    garbage_collection(heap);

    CU_ASSERT_TRUE(am_lookup(heap, tree));
    CU_ASSERT_FALSE(am_lookup(heap, tree_backup));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));
    
    h_delete(heap);
}

void test_traverse_and_copy_tree_struct_with_branches(){
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    tree_t *tree = create_tree(heap);
    void *tree_backup = tree;
    page_t *page_tree = h_get_page(heap, pm_index_from_pointer(heap, tree));

    CU_ASSERT_TRUE(am_lookup(heap, tree));
    CU_ASSERT_TRUE(am_lookup(heap, tree_backup));
    CU_ASSERT_PTR_EQUAL(page_tree, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &tree, tree, true);
    garbage_collection(heap);

    CU_ASSERT_TRUE(am_lookup(heap, tree));
    CU_ASSERT_FALSE(am_lookup(heap, tree_backup));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));

    h_delete(heap);
}

void test_traverse_and_copy_looping_structure() {
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    pointer_t *object = h_alloc_struct(heap, "*");
    void **child = h_alloc_struct(heap, "*");
    pointer_t *object_backup = object;
    void **child_backup = child;
    object->ptr = child;
    *child = object;
    page_t *page_object = h_get_page(heap, pm_index_from_pointer(heap, object));
    page_t *page_child = h_get_page(heap, pm_index_from_pointer(heap, child));

    CU_ASSERT_TRUE(am_lookup(heap, object));
    CU_ASSERT_TRUE(am_lookup(heap, child));
    CU_ASSERT_TRUE(am_lookup(heap, object_backup));
    CU_ASSERT_TRUE(am_lookup(heap, child_backup));
    CU_ASSERT_TRUE(page_object == page_child);
    CU_ASSERT_PTR_EQUAL(page_object, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &object, object, true);
    garbage_collection(heap);

    CU_ASSERT_TRUE(am_lookup(heap, object));
    void *new_child = object->ptr;
    CU_ASSERT_TRUE(am_lookup(heap, new_child));
    CU_ASSERT_FALSE(am_lookup(heap, object_backup));
    CU_ASSERT_FALSE(am_lookup(heap, child_backup));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));

    h_delete(heap);
}

void test_traverse_and_copy_unsafe_stack_pointer_single_child() {
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    pointer_t *object = h_alloc_struct(heap, "*");
    int *child = h_alloc_struct(heap, "i");
    object->ptr = child; 
    pointer_t *object_backup = object;
    int *child_backup = child;
    page_t *page_object = h_get_page(heap, pm_index_from_pointer(heap, object));
    page_t *page_child = h_get_page(heap, pm_index_from_pointer(heap, (void *)child));

    CU_ASSERT_TRUE(am_lookup(heap, object));   
    CU_ASSERT_TRUE(am_lookup(heap, child));
    CU_ASSERT_TRUE(page_object == page_child);
    CU_ASSERT_PTR_EQUAL(page_object, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &object, object, false);

    CU_ASSERT_TRUE(am_lookup(heap, object_backup));
    CU_ASSERT_FALSE(am_lookup(heap, (void *)child_backup));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    garbage_collection(heap);

    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));
    h_delete(heap);
}

void test_traverse_and_copy_unsafe_stack_pointer_looping_structure() {
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    pointer_t *object = h_alloc_struct(heap, "*");
    void **child = h_alloc_struct(heap, "*");
    pointer_t *object_backup = object;
    void **child_backup = child;
    object->ptr = child;
    *child = object;
    page_t *page_object = h_get_page(heap, pm_index_from_pointer(heap, object));
    page_t *page_child = h_get_page(heap, pm_index_from_pointer(heap, child));

    CU_ASSERT_TRUE(am_lookup(heap, object));
    CU_ASSERT_TRUE(am_lookup(heap, child));
    CU_ASSERT_TRUE(am_lookup(heap, object_backup));
    CU_ASSERT_TRUE(am_lookup(heap, child_backup));
    CU_ASSERT_TRUE(page_object == page_child);
    CU_ASSERT_PTR_EQUAL(page_object, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &object, object, false);
    garbage_collection(heap);

    CU_ASSERT_TRUE(am_lookup(heap, object));
    void *new_child = object->ptr;
    CU_ASSERT_TRUE(am_lookup(heap, new_child));
    CU_ASSERT_TRUE(am_lookup(heap, object_backup));
    CU_ASSERT_FALSE(am_lookup(heap, child_backup));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));
    CU_ASSERT_PTR_EQUAL(object, object_backup);
    CU_ASSERT_PTR_NOT_EQUAL(child, object->ptr);

    h_delete(heap);
}

void test_traverse_and_copy_non_word_aligned_pointer() {
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    int_and_ptr_t *structure = h_alloc_struct(heap, "i*");
    int *child = h_alloc_struct(heap, "i");
    structure->num = 42;
    structure->ptr = child;
    int_and_ptr_t *structure_backup = structure;
    int *child_backup = child;
    page_t *page_structure = h_get_page(heap, pm_index_from_pointer(heap, structure));
    page_t *page_child = h_get_page(heap, pm_index_from_pointer(heap, child));

    CU_ASSERT_TRUE(am_lookup(heap, structure));
    CU_ASSERT_TRUE(am_lookup(heap, child));
    CU_ASSERT_TRUE(am_lookup(heap, structure_backup));
    CU_ASSERT_TRUE(page_structure == page_child);
    CU_ASSERT_PTR_EQUAL(page_structure, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &structure, structure, true);
    garbage_collection(heap);
    
    CU_ASSERT_TRUE(am_lookup(heap, structure));
    void *new_child = structure->ptr;
    CU_ASSERT_TRUE(am_lookup(heap, new_child));
    CU_ASSERT_FALSE(am_lookup(heap, structure_backup));
    CU_ASSERT_FALSE(am_lookup(heap, child_backup));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));
    CU_ASSERT_PTR_NOT_EQUAL(structure, structure_backup);
    CU_ASSERT_PTR_NOT_EQUAL(child, structure->ptr);

    h_delete(heap);

}

void test_traverse_and_copy_multi_pointer_struct() {
    heap_t *heap = h_init(MIN_HEAP_SIZE, false, 1.0);
    multi_ptr_t *structure = h_alloc_struct(heap, "i**i*");
    int *child1 = h_alloc_struct(heap, "i");
    int *child2 = h_alloc_struct(heap, "i");
    int *child3 = h_alloc_struct(heap, "i");
    structure->num1 = 42;
    structure->num2 = 23;
    structure->ptr1 = child1;
    structure->ptr2 = child2;
    structure->ptr3 = child3;
    multi_ptr_t *structure_backup = structure;
    page_t *page_structure = h_get_page(heap, pm_index_from_pointer(heap, structure));
    page_t *page_child1 = h_get_page(heap, pm_index_from_pointer(heap, child1));
    page_t *page_child2 = h_get_page(heap, pm_index_from_pointer(heap, child2));
    page_t *page_child3 = h_get_page(heap, pm_index_from_pointer(heap, child3));

    CU_ASSERT_TRUE(am_lookup(heap, structure));
    CU_ASSERT_TRUE(am_lookup(heap, child1));
    CU_ASSERT_TRUE(am_lookup(heap, child2));
    CU_ASSERT_TRUE(am_lookup(heap, child3));
    CU_ASSERT_TRUE(page_structure == page_child1);
    CU_ASSERT_TRUE(page_structure == page_child2);
    CU_ASSERT_TRUE(page_structure == page_child3);
    CU_ASSERT_PTR_EQUAL(page_structure, h_get_page(heap, 0));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 1)));

    traverse_and_copy(heap, (void **) &structure, structure, true);
    garbage_collection(heap);

    CU_ASSERT_TRUE(am_lookup(heap, structure));
    CU_ASSERT_TRUE(am_lookup(heap, structure->ptr1));
    CU_ASSERT_TRUE(am_lookup(heap, structure->ptr2));
    CU_ASSERT_TRUE(am_lookup(heap, structure->ptr3));
    CU_ASSERT_FALSE(am_lookup(heap, structure_backup));
    CU_ASSERT_FALSE(am_lookup(heap, child1));
    CU_ASSERT_FALSE(am_lookup(heap, child2));
    CU_ASSERT_FALSE(am_lookup(heap, child3));
    CU_ASSERT_FALSE(pm_page_is_active(heap, h_get_page(heap, 0)));
    CU_ASSERT_TRUE(pm_page_is_active(heap, h_get_page(heap, 1)));
    CU_ASSERT_PTR_NOT_EQUAL(structure, structure_backup);
    CU_ASSERT_PTR_NOT_EQUAL(child1, structure->ptr1);
    CU_ASSERT_PTR_NOT_EQUAL(child2, structure->ptr2);
    CU_ASSERT_PTR_NOT_EQUAL(child3, structure->ptr3);

    h_delete(heap);

}

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    CU_pSuite traverse_and_copy_tests = CU_add_suite("Testing traverse_and_copy functionality", init_suite, clean_suite);
    if (traverse_and_copy_tests == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
            (CU_add_test(traverse_and_copy_tests, "Test object with forward address header", test_traverse_and_copy_forwarded_object) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test object without forward address header", test_traverse_and_copy_not_forwarded_object) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test object with single child", test_traverse_and_copy_single_child) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test object without forward address header with forwarded child", test_traverse_and_copy_unforwarded_object_with_forwarded_child) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test traverse and copy tree struct", test_traverse_and_copy_tree_struct) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test traverse and copy tree struct_with_branches", test_traverse_and_copy_tree_struct_with_branches) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test traverse and copy looping structure", test_traverse_and_copy_looping_structure) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test traverse and copy with an unsafe satck pointer - single child object", test_traverse_and_copy_unsafe_stack_pointer_single_child) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test traverse and copy with an unsafe satck pointer - looping structure", test_traverse_and_copy_unsafe_stack_pointer_looping_structure) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test traverse and copy with a struct with a non-word aligned pointer", test_traverse_and_copy_non_word_aligned_pointer) == NULL) ||
            (CU_add_test(traverse_and_copy_tests, "Test traverse and copy with a struct with a multi-pointer struct", test_traverse_and_copy_multi_pointer_struct) == NULL) ||
            0)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
