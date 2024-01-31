#include <stdbool.h>
#include "../gc.h"

/// @brief Determines if a character is an ascii-character representing a digit
/// @param c The character which will be checked
/// @return true if c is a ascii-digit, otherwise false
bool is_ascii_number(char c);

/// @brief Allocates a string to the given heap
/// @param heap The heap where the string will be allocated
/// @param string The string which will be copied over
/// @return Pointer to the string which has become allocated on the heapo
char *duplicate_string(heap_t * heap, char *string);

bool string_contains_character(char *string, char c);
