#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "../util/binary_util.h"


typedef uint64_t bitvector_t;

#define BITVECTOR_SIZE 64       // Size of bitvector in bits

/// @brief Create a 64-bit bitvector initialised to the binary representation of a number
/// @param initial_value The number whose binary representation the bitvector will be initialised to
/// @return The newly created and initialised bitvector
bitvector_t bitvector_create(uint64_t initial_value);

/// @brief Create an array of bitvectors each set to the binary representation of a particular number
/// @param bitvector_array Pointer to the address where the array will be written to. Should have space for 8*length bytes 
/// @param initial_values Integer array of the initial values for the bitvectors, in order of initialisation
/// NOTE: Calling with an integer array that differs in length to the bitvector array is undefined behaviour
/// @param length Number of bitvectors in the array
void bitvector_array_create(bitvector_t * bitvector_array,
                            uint64_t * initial_values, size_t length);

/// @brief Set a specific bit in a 64-bit bitvector to a given value
/// @param bitvector Pointer to the bitvector to be manipulated
/// @param index Index of the bit to be manipulated (beginning at 0)
/// @param value Value that the bit should be set to
void bitvector_set_bit(bitvector_t * bitvector, int index, bool value);

/// @brief Set a specific bit in an array of 64-bit bitvectors to a given value.
///        Assumes that the index given is valid.
/// @param bitvector_array The array of bitvectors to be manipulated
/// @param index Overall index in the array of the bit to be manipulated (beginning at 0)
/// EXAMPLE: Secong BV in array, second index = 65
/// @param value Value that the bit should be set to
void bitvector_array_set_bit(bitvector_t * bitvector_array, int index,
                             bool value);

/// @brief Return the value of the bit in a 64-bit bitvector at a given index
/// @param bitvector The bitvector whose bit shall be read
/// @param index The index of the bit in the bitvector
/// @return true if the bit is 1, otherwise false
bool bitvector_read_bit(bitvector_t bitvector, int index);

/// @brief Return the value of the bit in an array of 64-bit bitvectors at a given index
///        Assumes that the index given is valid.
/// @param bitvector_array The array of bitvectors to be read
/// @param index The overall index of the bit to be read
/// EXAMPLE: Secong BV in array, second index = 65
/// @return true if the bit is 1, otherwise false
bool bitvector_array_read_bit(bitvector_t * bitvector_array, int index);

/// @brief Count the number of bits that are set to 1 in a 64-bit bitvector
/// @param bitvector The bitvector whose bits will be counted
/// @return The number of bits in that bitvector that are set to 1
size_t bitvector_count_ones(bitvector_t bitvector);

/// @brief Count the number of bits set to 1 in an array of 64-bit bitvectors
/// @param bitvector_array The array of bitvectors to be counted
/// @param array_length Number of 64-bit bitvectors in the array
/// @return The total number of bits set to 1 in all the bitvectors in the array
size_t bitvector_array_count_ones(bitvector_t * bitvector_array,
                                  size_t array_length);
