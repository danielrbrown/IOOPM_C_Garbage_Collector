#pragma once
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils.h"
#include "../../src/gc.h"

/**
 * @file common.h
 * @author Simon Eriksson and Edvard Axelman
 * @brief Contains common tools used by the warehouse, shelves and frontend.
 */

/// @brief Flag used by merch_edit in warehouse
#define EDIT_NAME  0b001
/// @brief Flag used by merch_edit in warehouse
#define EDIT_PRICE 0b010
/// @brief Flag used by merch_edit in warehouse
#define EDIT_DESC  0b100
/// @brief Number of buckets in the hashtable
#define No_Buckets 17

/// @brief Macro for creating an ioopm_elem_t with an int value.
/// @param x value of type int
/// @returns an ioopm_elem_t with an int-value (.int_elem) set to x.
#define Int_elem(x) (ioopm_elem_t) { .int_elem=(x) }

/// @brief Macro for creating an ioopm_elem_t with a void pointer value.
/// @param x value of type void pointer
/// @returns an ioopm_elem_t with a void pointer-value (.ptr_elem) set to x.
#define Ptr_elem(x) (ioopm_elem_t) { .ptr_elem=(x) }

/// @brief Macro for creating an ioopm_elem_t with a char value.
/// @param x value of type char
/// @returns an ioopm_elem_t with a void pointer-value (.ptr_elem) set to x.
#define Char_elem(x) (ioopm_elem_t) { .char_elem=(x) }

/// @brief Macro for creating an ioopm_elem_t with a merch value.
/// @param x value of type merch_t
/// @returns an ioopm_elem_t with a merch value (.merch_elem) set to x.
#define Merch_elem(x) (ioopm_elem_t) { .merch_elem=(x) }

/// @brief Macro for creating an ioopm_elem_t with a shelf value.
/// @param x value of type shelf_t
/// @returns an ioopm_elem_t with a shelf value (.shelf_elem) set to x.
#define Shelf_elem(x) (ioopm_elem_t) { .shelf_elem=(x) }

/// @brief Macro for creating an ioopm_elem_t with a shelf value.
/// @param x value of type cart_t
/// @returns an ioopm_elem_t with a shelf value (.cart_elem) set to x.
#define Cart_elem(x) (ioopm_elem_t) { .cart_elem=(x) }

/// @brief Macro for creating an ioopm_elem_t with a shelf value.
/// @param x value of type cart_t
/// @returns an ioopm_elem_t with a shelf value (.cart_elem) set to x.
#define Cart_item_elem(x) (ioopm_elem_t) { .cart_item_elem=(x) }


/// @brief Macro for creating an ioopm_option_t with fields success=true and 
/// value=v
/// @param v value of type ioopm_elem_t
/// @returns an ioopm_option_t with .success = true and .value = v
#define Success(v)      (ioopm_option_t) { .success = true, .value = v }

/// @brief Macro for creating an ioopm_option_t with fields success=false and 
/// value=NULL
/// @returns an ioopm_option_t with .success = false and .value = NULL
#define Failure()       (ioopm_option_t) { .success = false }

/// @brief Macro for checking if an ioopm_option_t has a value
/// @param o ioopm_option_t
/// @returns true if the operation succeeded
#define Successful(o)   (o.success == true)

/// @brief Macro for checking if an ioopm_option_t does not have a value
/// @param o ioopm_option_t
/// @returns true if the operation failed
#define Unsuccessful(o) (o.success == false)

///@struct Elem
///@brief Element of some datatype
///@param int_elem element of int datatype
///@param ptr_elem element of ptr datatype
///@param merch_elem element of merch_t datatype
///@param shelf_elem element of shelf_t datatype
typedef union elem ioopm_elem_t;

///@struct Shelf
///@brief A shelf in the warehouse
///@param shelf_name name of the shelf
///@param amount amount of merchandise in the shelf
typedef struct shelf ioopm_shelf_t;

///@struct merch
///@brief A merchandise
///@param name Name of the merchandise
///@param desc Description of the merchandise
///@param price The price of the merchandise 
///@param shelf The location in the warehouse the merchandise is stored 
typedef struct merch ioopm_merch_t;

/// @struct hash_table
/// @brief A simple hash table with 17 buckets.
typedef struct hash_table ioopm_hash_table_t;

///@struct list
///@brief A linked list.
///@param first Pointer to the first link in the linked list.
///@param last Pointer to the last link in the linked list.
///@param eq_fun Function for comparing links for equality. 
///@param length Number of links in the linked list.  
typedef struct list ioopm_list_t;

///@struct link
///@brief Element in the linked list
///@param value The value held by the link
///@param next pointer to the next link. Is NULL if there is no link after it. 
typedef struct link ioopm_link_t;

/// @brief Data structure representing a shopping cart
typedef struct cart ioopm_cart_t;

/// @brief Data structure representing a list of shopping carts
typedef struct cart_list ioopm_cart_list_t;

/// @brief Data structure representing an item in a shopping cart
typedef struct cart_item ioopm_cart_item_t;

/// @brief Option type struct
/// @param success bool indicating whether the function returning the struct 
/// produced the intended data.
/// @param value ioopm_elem_t containing the value as some kind of datatype
typedef struct option ioopm_option_t;

/// @brief Equals function
typedef bool (*ioopm_eq_function)(ioopm_elem_t a, ioopm_elem_t b);

/// @brief List iterator type
typedef struct iter ioopm_list_iterator_t; 

union elem
{
    int int_elem;
    char char_elem;
    void *ptr_elem;
    ioopm_merch_t *merch_elem;
    ioopm_shelf_t *shelf_elem;
    ioopm_cart_t *cart_elem;
    ioopm_cart_item_t *cart_item_elem;
};

///@struct Merch
///@brief A datatype that represents a merchandise and contains all it's important values and parameters
///@param name A string, the name of the merchandise
///@param locs A pointer to linked list that contains all the shelves where that merch is stored and how many at each shelf
///@param price An integer that assigns the price of the merchandise
///@param stock_in_cart An integer that represents how many of the merchandise is in a cart
///@param total_stock An integer that represents the total stock of the merchandise
///@param desc A string, the description of the merchandise
struct merch
{
    char *name;
    ioopm_list_t *locs;
    int price;
    int stock_in_cart;
    int total_stock;
    char *desc;
};

///@struct Shelf
///@brief A datatype that contains a string and a positive integer
///@param shelf_name The name of the shelf
///@param amount An positive integer that represents the amount of merch that is stored on the shelf
struct shelf
{
    char *shelf_name;
    uint32_t amount;
};


///@struct Link
///@brief Element in the linked list
///@param value The value held by the link
///@param next Pointer to the next link. Is NULL if there is no link after it. 
struct link
{
    ioopm_elem_t element;
    ioopm_link_t *next;
};

///@struct Cart
///@brief Stores the products the user wants to checkout from the warehouse
///@param products Hash table that stores the products in the cart
///@param next Unique id for the cart 
struct cart
{
    ioopm_hash_table_t *products;
    int cart_id;
};

///@struct Cart item
///@brief An item that is stored in the cart hasht table (products)
///@param merch_name The name of the merch
///@param merch A pointer to the actual merch
///@param amount An integer that represents how many of that specific merch is in the cart
struct cart_item
{
    char *merch_name;
    ioopm_merch_t *merch;
    int amount;
};

///@struct Cart list
///@brief A linked list that contains cart_item as elements
///@param carts A linked list
///@param next_cart_id An integer that increases by one everytime we add a cart
///@param no_carts_removed An integer that keeps track on how many carts we have removed
struct cart_list
{
    ioopm_list_t *carts;
    int next_cart_id;
    int no_carts_removed;
};

///@struct option
///@brief checks success and returns a value if successful
///@param success set to true if function has returned a value
///@param value may be NULL if function did not succeed
struct option
{
  bool success;
  ioopm_elem_t value;
};

///@struct list
///@brief A linked list.
///@param first Pointer to the first link in the linked list.
///@param last Pointer to the last link in the linked list.
///@param eq_fun Function for comparing links for equality. 
///@param length Number of links in the linked list.
struct list
{
    heap_t *heap;
    
    ioopm_link_t *first;
    ioopm_link_t *last;
    ioopm_eq_function eq_fun; 
    int size;
};

/// @brief Takes a key and hashes it 
typedef int(*ioopm_hash_function_t)(ioopm_elem_t key);

/// @brief Compares two elements and returns true if they are equal
typedef bool(*ioopm_hash_eq_function_t)(ioopm_elem_t a, ioopm_elem_t b);

/// @brief Compares two elements and returns true if they are equal or bigger than
typedef bool(*ioopm_hash_comp_function_t)(ioopm_elem_t a, ioopm_elem_t b);

/// @brief Compares an element and a chosen data type
typedef bool(*ioopm_predicate)(ioopm_elem_t index ,ioopm_elem_t element, void *extra);

/// @brief Applies a function to a data-structure, only supports int elems
typedef void(*ioopm_apply_int_function)(ioopm_elem_t index, ioopm_elem_t *element, void *extra);

/// @brief Applies a function to a data-structure
typedef void(*ioopm_apply_function)(ioopm_elem_t index, ioopm_elem_t *element, void *extra);

/// @brief checks equality between two strings
/// @param a element being compared
/// @param b element being compared
/// @return true if string a and string b are equal
bool ioopm_string_eq(ioopm_elem_t str1, ioopm_elem_t str2);

/// @brief checks equality between two ints
/// @param a element being compared 
/// @param b element being compared 
/// @return true if a and b are equal
bool ioopm_int_eq(ioopm_elem_t int1, ioopm_elem_t int2);

/// @brief checks equality between two shelfs
/// @param a element being compared 
/// @param b element being compared 
/// @return true if a and b are equal and valid shelf(strings)
bool ioopm_shelf_eq(ioopm_elem_t shelf1, ioopm_elem_t shelf2);

/// @brief checks if a is bigger or equal b
/// @param a element being compared
/// @param b element being compared
/// @return true if a is equal or bigger than b
bool ioopm_comp_func(ioopm_elem_t a, ioopm_elem_t b);

/// @brief Check if string a is equal to b
/// @param a element being compared
/// @param b element being compared
/// @return true if a is equal to b
bool ioopm_comp_string_func(ioopm_elem_t a, ioopm_elem_t b);

/// @brief Simple hash function that takes a key and hashes it
/// @param key element being hashed
/// @return hashed key
int ioopm_hash_f(ioopm_elem_t key);

/// @brief Simple hash function for string elem_t's
int ioopm_string_sum_hash(ioopm_elem_t e);

/// @brief Equals function for merch elem_t's
bool ioopm_merch_eq(ioopm_elem_t merch1, ioopm_elem_t merch2);

/// @brief Simple hash function for merch elem_t's
int ioopm_merch_sum_hash(ioopm_elem_t merch);