#pragma once
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// @brief Converts a string which represents a 64-bit unsigned binary number to its integer value
// @return The integer value in the format uint64_t which the given string represents
// @param binary_string The string which represents the unsigned binary number which will be converted
uint64_t binary_string_to_uint64(char *binary_string);

/// @brief Prints out the binary representation of a uint64_t with the LSB on the right 
/// @param n A uint65_t variable
void print_uint64_as_binary(uint64_t n);

void print_heap(uint64_t * starting_adress, int words);
