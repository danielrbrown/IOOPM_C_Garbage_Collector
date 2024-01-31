#include <stdbool.h>
#include <stdlib.h>

/// @brief Expands a formatstring, replacing coefficients with that amount of that character
///        Example: "3*2i" -> "***ii"
/// @param format_string The format string which will be expanded
/// @param buffer The buffer where the expanded format string will be written to. Make sure you have a large enough buffer!
void format_string_expander(char *format_string, char buffer[]);

/// @brief Checks if a format string only has characters that represent primitive datatypes
/// @param format_string The format string which will be checked
/// @return true if the format string solely contains characters representing primitive datatypes, otherwise false
bool format_string_contains_only_primitive_datatypes(char *format_string);

/// @brief Checks if a format string contains only ascii-digits
/// @param format_string The format string which will be checked
/// @return true if the format string only contains ascii-digits, otherwise false
bool format_string_contains_only_numbers(char *format_string);

/// @brief Calculates the required space for a raw data alloc
/// @param format_string The format string which contains information for a raw data alloc
/// @return the size of the allocation needed to contain the allocation 
int expanded_format_string_required_space_for_data_alloc(char *format_string);

/// @brief Interprets a format string
size_t interpret_format_string(char *format_string, int *offset_array);

size_t format_string_letter_to_size(char letter);
