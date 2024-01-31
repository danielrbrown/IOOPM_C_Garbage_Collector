#pragma once
#include "utils.h"
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "common.h"
#include "shelves.h"

/**
 * @file warehouse.h
 * @author Simon Eriksson and Edvard Axelman
 * @brief Contains all logic related to the warehouse in the webstore assignment.
 */

/// @brief Creates a new empty warehouse that is represented as a hash table data structure
/// @param heap the heap
/// @return An empty warehouse table
ioopm_hash_table_t *ioopm_warehouse_create(heap_t *heap);

/// @brief Creates a new merchandise
/// @return A new merchandise with it's appurtenant data
ioopm_merch_t *ioopm_merch_create(char *name, char *desc, int price, heap_t *heap);

/// @brief Checks if a merch already exists in the warehouse
/// @param wh warehouse
/// @param merch_name name of the merch
/// @return true if merch with merch_name already exists in warehouse
bool ioopm_merch_already_in_warehouse(ioopm_hash_table_t *wh, char *merch_name);

/// @brief Inserts a merchandise into the warehouse
/// @return The merchandise that have been inserted
void ioopm_merch_insert(ioopm_hash_table_t *warehouse, ioopm_merch_t *merch);

/// @brief Destroys the whole warehouse
/// @param warehouse name of the warehouse 
void ioopm_warehouse_destroy(ioopm_hash_table_t *warehouse);

/// @brief Replenishes a merchandise in the warehouse
/// @param shelves Shelves hash table
/// @param merch Target merchandise
/// @param amount A positive integer, the amount of merch to be added
/// @param shelf_name A string, the name of the merch
void ioopm_merch_replenish(ioopm_hash_table_t *shelves ,ioopm_merch_t *merch, uint32_t amount, char *shelf_name);

/// @brief Checks if a shelf exist in merch-locations (linked list)
/// @param list A linked list 
/// @param element Shelf name
/// @return FALSE: If the shelf does not exist inside the linked list
/// @return TRUE: If the shelf exist inside the linked list
bool ioopm_shelf_contains(ioopm_list_t *list, ioopm_elem_t element);

/// @brief Destroys a merchandise and removes it from the warehouse hash table and shelves hash table
/// @param shelves Shelves hash table
/// @param warehouse Warehouse hash table
/// @param merch Target merchandise
void ioopm_product_destroy(ioopm_hash_table_t *shelves, ioopm_hash_table_t *warehouse, ioopm_merch_t *merch);

/// @brief Edit the name of the merchandise
/// @param wh Warehouse hash table
/// @param merch Target merchandise
/// @param new_name A string, the new name for the merchandise
void ioopm_merch_edit_name(ioopm_hash_table_t *wh, ioopm_merch_t *merch, char *new_name); 

/// @brief Edit the description of the merchandise
/// @param wh Warehouse hash table
/// @param merch Target merchandise
/// @param new_desc A string, the new description for the merchandise 
void ioopm_merch_edit_desc(ioopm_hash_table_t *wh, ioopm_merch_t *merch, char *new_desc); 

/// @brief Edit the price of the merchandise
/// @param wh Warehouse hash table
/// @param merch Target merchandise
/// @param new_price A integer, the new price for the merchandise
void ioopm_merch_edit_price(ioopm_hash_table_t *wh, ioopm_merch_t *merch, int new_price); 

/// @brief Edits a merch according to what flags are set, can include editing
/// the name, description and price, or any combination of the three.
/// @param wh Warehouse hash table
/// @param merch Target merchandise
/// @param new_name this name will be set if the flag for editing name is enabled.
/// @param new_desc this description will be set if the flag for editing description is enabled.
/// @param new_price this price will be set if the flag for editing price is enabled.
/// @param flags may use EDIT_PRICE, EDIT_NAME and EDIT_DESC or any combination of them using bitwise logic.
void ioopm_merch_edit(ioopm_hash_table_t *wh, ioopm_merch_t *merch, char *new_name, char *new_desc, int new_price, char flags); 

/// @brief Gets the index for a shelf in the merchandise locations (linked list)
/// @param merch The merchandise where the shelf belongs to
/// @param shelf_name A string, the name of the targeted shelf
/// @param heap the heap
/// @return A positive integer that represents the index of the shelf in the merchandise locations (linked list)
uint32_t ioopm_shelf_index(ioopm_merch_t *merch, char *shelf_name, heap_t *heap);

/// @brief Creates a cart list
/// @param heap the heap
/// @return A cart list
ioopm_cart_list_t *ioopm_cart_list_create(heap_t *heap);

/// @brief Adds a merchandise into a cart
/// @param cart_list A cart list
/// @param warehouse Warehouse hash table
/// @param merch Target merchandise
/// @param cart_id An integer that represents the cart id
/// @param amount An integer, the amount of merchandise to be added into the cart
/// @return FALSE: If the function failed to add the merchandise into a cart
/// @return TRUE: If the function succeeded to add the merchandise into a cart
bool ioopm_cart_add_merch(ioopm_cart_list_t *cart_list, ioopm_hash_table_t *warehouse, ioopm_merch_t *merch, uint32_t cart_id, uint32_t amount);

/// @brief Removes x amount of merchandise from a cart
/// @param cart_list A cart list
/// @param warehouse Warehouse hash table
/// @param merch Target merchandise
/// @param cart_id An integer that represents the cart id
/// @param amount_to_remove An integer, the amount of merchandise to be removed from the cart
void ioopm_cart_remove_merch(ioopm_cart_list_t *cart_list, ioopm_hash_table_t *warehouse, ioopm_merch_t *merch, uint32_t cart_id, uint32_t amount_to_remove);

/// @brief Adds a cart to the cart list
/// @param cart_list Target cart list
/// @param cart Target cart
void ioopm_cart_add_to_list(ioopm_cart_list_t *cart_list, ioopm_cart_t *cart);

/// @brief Destroys the cart list
/// @param cart_list Target cart list
void ioopm_cart_list_destroy(ioopm_cart_list_t *cart_list);

/// @brief Destroys a specific cart
/// @param cart_list Target cart list
/// @param cart_index An integer that represents the carts index in the cart list
void ioopm_cart_spec_destroy(ioopm_cart_list_t *cart_list, uint32_t cart_index);

/// @brief Calculates the total cost of a cart
/// @param wh Warehouse hash table
/// @param cart_list A cart list
/// @param cart_id An integer that represents the cart id
/// @return An integer that represents the total cost of the selected cart
uint32_t ioopm_cart_total_cost(ioopm_hash_table_t *wh, ioopm_cart_list_t *cart_list, uint32_t cart_id);

/// @brief Checks out a cart and removes x amount of merchandise(s) from the warehouse
/// @param warehouse Warehouse hash table
/// @param shelves Shelves hash table
/// @param cart_list A cart list
/// @param cart Targeted cart
void ioopm_cart_checkout(ioopm_hash_table_t *warehouse, ioopm_hash_table_t *shelves, ioopm_cart_list_t *cart_list, ioopm_cart_t *cart); //Utgår från att alla inputs är korrekta. 

/// @brief Creates a cart in the cart list
/// @param cart_list Cart list where the new cart is inserted into
void ioopm_cart_create(ioopm_cart_list_t *cart_list);

/// @brief Checks if there are any merchandise in the warehouse that is replenished
/// @param wh Warehouse hash table
/// @return FALSE: If there aren't any merchandise in the warehouse that is replenished
/// @return TRUE: If there are atleast one merchandise in the warehouse that is replenished
bool ioopm_merch_is_replenished(ioopm_hash_table_t *wh);

/// @brief Checks if there are any merchandise in the cart(s)
/// @param cart_list A cart list
/// @return FALSE: If there aren't any merchandise in the cart(s)
/// @return TRUE: If there are atleast one merchandise in the cart(s)
bool ioopm_merch_in_cart_any(ioopm_cart_list_t *cart_list);

