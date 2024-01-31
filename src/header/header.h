#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "../gc.h"

typedef uint64_t header_t;
typedef unsigned char bit;

/// @brief Creates a header for a struct. Information about what data will be in the struct has to
///        be provided through the format string.
/// @param format_string The format string
/// @param heap The heap (where the format string will be allocated if the metadata doesn't fit in a bitvector
/// @return the appropiate header for the struct
header_t header_creation_layout(char *format_string, heap_t * heap);

/// @brief Interprets a bitvector layout specification header or a header with pointer to format string
/// @param header The header for the given struct
/// @param offset_array Pointer to an array of integers where the offsets to the pointers will be saved.
///                     (When calling this function, it's your responsibility to make sure that the array
///                      is large enough to fit all the offsets.)
/// @return the amount of pointers in the given struct
size_t header_interpretation(header_t header, int *offset_array);

/// @brief Creates a header with a forwarding pointer. Used when moving struct from active to passive block.
/// @param forwarding_pointer The forwarding pointer which the header will contain
/// @return the appropriate header with the forwarding pointer.
header_t create_header_with_forwarding_pointer(void *forwarding_pointer);

/// @brief Decides the size of a raw allocation according to the header
/// @param header The header belonging to a raw allocation
/// @return size of the raw allocation in bytes
size_t size_of_raw_allocation(header_t header);

/// @brief Decides the size of a struct allocation according to the header
/// @param header The header beloning to a struct allocation
/// @return size of the struct allocation in bytes (DODGE: it is assumed that each field of a struct is 8 bytes)
size_t size_of_struct_allocation(header_t header);

/// @brief Extracts the forwarding pointer from a header containing one
/// @param header The header containing a forwarding pointer
/// @return the pointer contained in the header
void *get_forwarding_pointer_from_header(header_t header);

/// @brief Extracts the format string pointer from a header containing one
/// @param header The header containing a format string pointer
/// @return The format string pointer
char *get_format_string_from_header(header_t header);

/// @brief Checks if a header is a format string
/// @param header The header
/// @return true if the header is a format string, otherwise false
bool is_format_string_header(header_t header);

/// @brief Checks if a header is a forwarding address
/// @param header The header
/// @return true if the header is a forwarding address, otherwise false
bool is_forwarding_address_header(header_t header);

/// @brief Checks if a header is a struct layout
/// @param header The header
/// @return true if the header is a struct layout, otherwise false
bool is_struct_layout_header(header_t header);

/// @brief Checks if a header is a raw layout
/// @param header The header
/// @return true if the header is a raw layout, otherwise false
bool is_raw_layout_header(header_t header);

/// @brief Changes type of a header according to pattern
/// @param bitvector a header we want to change the type of
/// @param pattern FORMAT_STRING, FORWARDING_ADDRESS or BITVECTOR_LAYOUT
void change_header_type_to(bitvector_t * bitvector, int pattern);
