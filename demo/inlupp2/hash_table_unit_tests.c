#include <CUnit/Basic.h>
#include "hash_table.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

int init_suite(void) {
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void) {
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

//---DEFINITIONS
static bool div2(ioopm_elem_t key, ioopm_elem_t value_ignored, void *x)
{
  if((key.int_elem % 2) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
  
}

bool hash_table_key_div2(ioopm_hash_table_t *ht)
{
  void *arg = arg;

  bool result = ioopm_hash_table_all(ht, div2, arg);
  return result; 
}

void change_value(ioopm_elem_t key, ioopm_elem_t value, void *extra){
  /*ioopm_elem_t t_value = 
  ioopm_elem_t *other_value_ptr = &t_value; 
  ioopm_elem_t other_value = *other_value_ptr; */
}

void value_changed(ioopm_hash_table_t *ht){
  ioopm_elem_t new_value = Ptr_elem("a");
  ioopm_hash_table_apply_to_all(ht, change_value, new_value.ptr_elem);
}

//-----TEST FUNCTIONS-----
void test_create_destroy()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, NULL, NULL);
   CU_ASSERT_PTR_NOT_NULL(ht);
   ioopm_hash_table_destroy(ht);

}

void test_insert_once()
{
 ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
 
 ioopm_elem_t test_key = Int_elem(1);
 ioopm_elem_t test_value = Ptr_elem("bar");


 CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, test_key));
 
 ioopm_hash_table_insert(ht, test_key, test_value);

 CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(ht, test_key));


 ioopm_hash_table_destroy(ht);

}


void test_conflict_key()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_elem_t test_key1 = Int_elem(-1);
  ioopm_elem_t test_value1 = Ptr_elem("foo");

  ioopm_elem_t test_key2 = Int_elem(-1);
  ioopm_elem_t test_value2 = Ptr_elem("bar");

  ioopm_hash_table_insert(ht, test_key1, test_value1);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, test_key1)->ptr_elem, "foo");
  ioopm_hash_table_insert(ht, test_key2, test_value2);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, test_key2)->ptr_elem, "bar");
  

  ioopm_hash_table_destroy(ht);
}

void test_search_multiple_entries()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);

  ioopm_elem_t test_key1 = Int_elem(0);
  ioopm_elem_t test_value1 = Ptr_elem("foo"); 

  ioopm_elem_t test_key2 = Int_elem(17);
  ioopm_elem_t test_value2 = Ptr_elem("bar");

  ioopm_hash_table_insert(ht, test_key1, test_value1);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, test_key1)->ptr_elem, "foo");
  ioopm_hash_table_insert(ht, test_key2, test_value2);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, test_key2)->ptr_elem, "bar");

  ioopm_hash_table_destroy(ht);
}

void test_remove_invalid_entry()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);

  ioopm_elem_t test_key1 = Int_elem(0);
  ioopm_elem_t test_value1 = Ptr_elem("foo"); 

  ioopm_elem_t test_key2 = Int_elem(17);
  ioopm_elem_t test_value2 = Ptr_elem("bar");

  ioopm_hash_table_insert(ht, test_key1, test_value1);
  ioopm_hash_table_insert(ht, test_key2, test_value2);
  CU_ASSERT_PTR_NULL(ioopm_hash_table_remove(ht, Ptr_elem("non valid entry")));

  ioopm_hash_table_destroy(ht);
}

void test_string_eq_func()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_string_sum_hash, ioopm_comp_string_func, ioopm_string_eq);

  ioopm_elem_t test_key1 = Ptr_elem("foo");
  ioopm_elem_t test_value1 = Int_elem(-3); 
  
  ioopm_elem_t test_key2 = Ptr_elem("bar");
  ioopm_elem_t test_value2 = Int_elem(6); 

  ioopm_elem_t test_key3 = Ptr_elem("foo");
  ioopm_elem_t test_value3 = Int_elem(10); 

  ioopm_hash_table_insert(ht, test_key1, test_value1);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, test_key1)->int_elem, -3);
  ioopm_hash_table_insert(ht, test_key2, test_value2);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, test_key2)->int_elem, 6);
  ioopm_hash_table_insert(ht, test_key3, test_value3);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, test_key3)->int_elem, 10);
  CU_ASSERT_NOT_EQUAL(ioopm_hash_table_lookup(ht, test_key3)->int_elem, 0);

  ioopm_hash_table_destroy(ht);
}

void test_lookup_empty()
{
   ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
   for (int i = 0; i < 18; ++i) /// 18 is a bit magical
     {
       CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, Int_elem(i)));
     }
   CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, Int_elem(-1)));
   ioopm_hash_table_destroy(ht);
}

void test_destroy_entry()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_elem_t test_key1 = Int_elem(1);
  ioopm_elem_t test_value1 = Ptr_elem("foo");
  ioopm_hash_table_insert(ht, test_key1, test_value1);
  ioopm_hash_table_remove(ht, test_key1);
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, test_key1));
  ioopm_hash_table_destroy(ht);

}

void test_destroy_non_valid_entry()
{
  
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_elem_t test_key1 = Int_elem(1);
  ioopm_elem_t test_value1 = Ptr_elem("foo");
  ioopm_hash_table_insert(ht, test_key1, test_value1);
  ioopm_hash_table_remove(ht, test_key1);

  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, Int_elem(10))); //Non existent key
  ioopm_hash_table_destroy(ht);
}

void test_size_of_table()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_elem_t test_key1 = Int_elem(1);
  ioopm_elem_t test_value1 = Ptr_elem("foo");

  ioopm_hash_table_insert(ht, test_key1, test_value1);
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 1);
  ioopm_hash_table_destroy(ht);
}

void test_size_of_empty_table()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0);
  ioopm_hash_table_destroy(ht);
}

void test_check_if_empty_table()
{
  ioopm_elem_t test_key = Int_elem(1);
  ioopm_elem_t test_value = Ptr_elem("foo");
  
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));
  ioopm_hash_table_insert(ht, test_key, test_value);
  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));
  ioopm_hash_table_destroy(ht);
}

void test_clear_table()
{
  ioopm_elem_t test_key1 = Int_elem(1);
  ioopm_elem_t test_value1 = Ptr_elem("foo");

  ioopm_elem_t test_key2 = Int_elem(5);
  ioopm_elem_t test_value2 = Ptr_elem("test");
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_hash_table_insert(ht, test_key1, test_value1);
  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));
  ioopm_hash_table_insert(ht, test_key2, test_value2);
  CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));
  ioopm_hash_table_clear(ht);
  CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));
  ioopm_hash_table_destroy(ht);

}

void test_ht_keys2()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_hash_table_insert(ht, Int_elem(10), Char_elem('a'));
  ioopm_hash_table_insert(ht, Int_elem(4), Char_elem('b'));
  ioopm_hash_table_insert(ht, Int_elem(7), Char_elem('c'));

  ioopm_list_t *key_list = ioopm_hash_table_keys(ht);

  ioopm_elem_t i2 = ioopm_linked_list_get(key_list, 0).value;
  CU_ASSERT_FALSE(ioopm_linked_list_get(key_list, 4).success); //Out of bound testing.
  CU_ASSERT_EQUAL(i2.int_elem, 4); 

  ioopm_linked_list_destroy(key_list);
  ioopm_hash_table_destroy(ht);

}

void test_ht_keys()
{

  int keys[3] = {10, 4, 7};
  bool found[3] = {false, false, false};

  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);

  ioopm_hash_table_insert(ht, Int_elem(10), Char_elem('a'));
  ioopm_hash_table_insert(ht, Int_elem(4), Char_elem('b'));
  ioopm_hash_table_insert(ht, Int_elem(7), Char_elem('c'));
  ioopm_list_t *key_list = ioopm_hash_table_keys(ht);


  for(int i = 0; i < 3; i++)
  {
    
  if(ioopm_linked_list_get(key_list, (i)).value.int_elem != keys[i])
  {
    for(int k = 0; k < 3; k++)
    {
      if(ioopm_linked_list_get(key_list, i).value.int_elem == keys[k])
      {
        found[i] = true;
      }
    }
  }
  
  else if(ioopm_linked_list_get(key_list, i).value.int_elem == keys[i])
  {
    found[i] = true;
  }

  else
  {
    CU_FAIL("Found a key that was never inserted!");
  }
  }
  for(int j = 0; j < 3; j++)
  {
    ioopm_elem_t curr_int = ioopm_linked_list_get(key_list, j).value;
    CU_ASSERT_TRUE(found[j]);
  }
  int sizeofht = ioopm_hash_table_size(ht);
  
  ioopm_linked_list_destroy(key_list);
  ioopm_hash_table_destroy(ht);
 
}

void test_ht_values()

{
  int keys[3] = {15, 28, 6};
  char *values[3] = {"hash", "table", "test"};
  bool found[3] = {false, false, false};
  bool found_value[3] = {false, false, false};

  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);

  ioopm_hash_table_insert(ht, Int_elem(15), Ptr_elem("hash"));
  ioopm_hash_table_insert(ht, Int_elem(28), Ptr_elem("table"));
  ioopm_hash_table_insert(ht, Int_elem(6), Ptr_elem("test"));

  ioopm_list_t *key_list = ioopm_hash_table_keys(ht);
  ioopm_elem_t *value_array = ioopm_hash_table_values(ht);

  for(int i = 0; i < 3; i++)
  {
    
    for(int k = 0; k < 3; k++)
    {
      if(keys[i] == ioopm_linked_list_get(key_list, k).value.int_elem)
      {
        found[i] = true;
      }
      if(strcmp(values[i], value_array[k].ptr_elem) == 0 ) //TESTAR ENDAST STRINGS
      {
        found_value[i] = true;
      }

    }
  }
  for(int y = 0; y < 3; y++)
  {
    CU_ASSERT_TRUE(found[y]);
    CU_ASSERT_TRUE(found_value[y]);
  }

  ioopm_hash_table_destroy(ht);
  ioopm_linked_list_destroy(key_list);
  free(value_array);
}
  



void test_ht_key_exist()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_hash_table_insert(ht, Int_elem(15), Ptr_elem("hash"));
  ioopm_hash_table_insert(ht, Int_elem(28), Ptr_elem("table"));
  ioopm_hash_table_insert(ht, Int_elem(28), Ptr_elem("test"));
  
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, Int_elem(15)));
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, Int_elem(28)));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, Int_elem(-1)));
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, Int_elem(34839241))); 
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, Int_elem(10)));
  ioopm_hash_table_destroy(ht);

}


void test_ht_value_exist()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_hash_table_insert(ht, Int_elem(15), Ptr_elem("hash"));
  ioopm_hash_table_insert(ht, Int_elem(28), Ptr_elem("table"));
  ioopm_hash_table_insert(ht, Int_elem(1), Ptr_elem("test"));
  ioopm_hash_table_insert(ht, Int_elem(87), Ptr_elem(" "));

  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, Ptr_elem("hash")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, Ptr_elem("test")));
  ioopm_hash_table_insert(ht, Int_elem(15), Ptr_elem("falsehash"));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, Ptr_elem("falsehash")));
  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, Ptr_elem("hash")));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, Ptr_elem(" ")))

  char *zero = strdup("zero");
  char *ten = strdup("ten");
  char *blank = strdup(" ");
  ioopm_hash_table_insert(ht, Int_elem(78), Ptr_elem("zero"));
  ioopm_hash_table_insert(ht, Int_elem(43), Ptr_elem("ten"));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, Ptr_elem(zero)));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, Ptr_elem(ten)));
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, Ptr_elem(blank)));

  free(zero);
  free(ten);
  free(blank);

  ioopm_hash_table_destroy(ht);
}



void test_ht_all_key()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  ioopm_hash_table_insert(ht, Int_elem(22), Ptr_elem("test1"));
  ioopm_hash_table_insert(ht, Int_elem(6), Ptr_elem("test2"));
  ioopm_hash_table_insert(ht, Int_elem(10), Ptr_elem("test3"));

  CU_ASSERT_TRUE(hash_table_key_div2(ht));
  ioopm_hash_table_insert(ht, Int_elem(9), Ptr_elem("test3"));
  CU_ASSERT_FALSE(hash_table_key_div2(ht));
  ioopm_hash_table_remove(ht, Int_elem(9));
  CU_ASSERT_TRUE(hash_table_key_div2(ht));


  ioopm_hash_table_destroy(ht);

}

void test_ht_apply()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(ioopm_hash_f, ioopm_comp_func, ioopm_int_eq);
  int keys[3] = {2,4,6};

  char *str1 = strdup("t ");

  for(int i = 0; i<3; i++){
    ioopm_hash_table_insert(ht, Int_elem(keys[i]), Ptr_elem(str1));
  }


  value_changed(ht);

  ioopm_elem_t *values = ioopm_hash_table_values(ht);

  char *str2 = strdup("a");
  for (int i=0; i<3; i++){
    CU_ASSERT_STRING_EQUAL(values[i].ptr_elem, *str2);
  }

  char *str3 = strdup("  ");
  for (int i=0; i<3; i++){
    CU_ASSERT_STRING_NOT_EQUAL(values[i].ptr_elem, *str3);
  }
  
  free(values);
  free(str1);
  free(str2);
  free(str3);
  ioopm_hash_table_destroy(ht);
} 


//----------------------------------------------------------------
int main() {
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite my_test_suite = CU_add_suite("My awesome test suite", init_suite, clean_suite);
  if (my_test_suite == NULL) {
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
    (CU_add_test(my_test_suite, "Test-destruct of hashtable", test_create_destroy) == NULL) ||
    (CU_add_test(my_test_suite, "A simple insert test", test_insert_once) == NULL) ||
    (CU_add_test(my_test_suite, "Test conflict key", test_conflict_key) == NULL) ||
    (CU_add_test(my_test_suite, "Test search multiple entries", test_insert_once) == NULL) || 
    (CU_add_test(my_test_suite, "Test string eq", test_string_eq_func) == NULL) || 
    (CU_add_test(my_test_suite, "Test multiple key on same bucket", test_search_multiple_entries) == NULL) ||
    (CU_add_test(my_test_suite, "Test remove non valid entry", test_remove_invalid_entry) == NULL) ||
    (CU_add_test(my_test_suite, "Test lookup an empty hash table", test_lookup_empty) == NULL) ||
    (CU_add_test(my_test_suite, "Test destroy an entry", test_destroy_entry) == NULL) ||
    (CU_add_test(my_test_suite, "Test destroy an invalid entry", test_destroy_non_valid_entry) == NULL) ||
    (CU_add_test(my_test_suite, "Test size of table", test_size_of_table) == NULL) ||
    (CU_add_test(my_test_suite, "Test size of empty table", test_size_of_empty_table) == NULL) ||
    (CU_add_test(my_test_suite, "Checks if table is empty", test_check_if_empty_table) == NULL) ||  
    (CU_add_test(my_test_suite, "Test clear table", test_clear_table) == NULL) ||
    (CU_add_test(my_test_suite, "Test key array V1", test_ht_keys2) == NULL) ||
    (CU_add_test(my_test_suite, "Test key array V2", test_ht_keys) == NULL) ||
    (CU_add_test(my_test_suite, "Test value array", test_ht_values) == NULL) ||
    (CU_add_test(my_test_suite, "Test key exist", test_ht_key_exist) == NULL) ||
    (CU_add_test(my_test_suite, "Test value exist", test_ht_value_exist) == NULL) ||
    (CU_add_test(my_test_suite, "Test if all keys is divisible", test_ht_all_key) == NULL) ||
    0

    
  )
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

