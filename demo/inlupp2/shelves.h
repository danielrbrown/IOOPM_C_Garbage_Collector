#pragma once
#include "utils.h"
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "warehouse.h"
#include "common.h"

/**
 * @file shelves.h
 * @author Simon Eriksson and Edvard Axelman
 * @brief Contains all logic related to shelves for the webstore assignment.
 */

/// @brief Creates a bunch of empty storage shelves that is represented as a hash table data structure
/// @return Empty storage shelves
ioopm_hash_table_t *ioopm_shelves_create(heap_t *heap);

/// @brief Creates a shelf struct
/// @param shelf_name_input A string, the name of the shelf
/// @param amount An integer, the amount of merch in the shelf
/// @return A shelf 
ioopm_shelf_t *ioopm_shelf_create(char *shelf_name_input, uint32_t amount, heap_t *heap);

/// @brief Insert a shelf into the shelf locations for a merch
/// @param shelf_locations A linked list 
/// @param shelf A shelf
void ioopm_merch_insert_location(ioopm_list_t *shelf_locations, ioopm_shelf_t *shelf);

/// @brief Inserts a shelf as a key and a merch as a value into the shelves hash table
/// @param shelves Shelves hash table
/// @param shelf A string, the name of the shelf
/// @param merch Target merch
void ioopm_shelf_insert(ioopm_hash_table_t *shelves, char *shelf, ioopm_merch_t *merch);

/// @brief Checks if a shelf name is free for a specific merch
/// @param shelves Shelves hash table
/// @param shelf_name A string, the name of the shelf
/// @param merch Target merch
/// @returns FALSE: if the shelf name exist and it does not exist in the merchandise location (linked list) 
/// @returns TRUE: if the name does not exist or it exist in the merchandise location (linked list)
bool ioopm_is_shelf_free(ioopm_hash_table_t *shelves, char *shelf_name, ioopm_merch_t *merch);

/// @brief Removes a specific shelf from the shelves hash table
/// @param shelves Shelves hash table
/// @param shelf_name A string, the name of the shelf
void ioopm_shelf_remove(ioopm_hash_table_t *shelves, char *shelf_name);

/// @brief Destroys the shelves hash table
/// @param shelves Shelves hash table
void ioopm_shelves_destroy(ioopm_hash_table_t *shelves); 
