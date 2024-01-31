#include "../gc.h"
#include <string.h>

bool is_ascii_number(char c)
{
    return '0' <= c && c <= '9';
}


char *duplicate_string(heap_t * heap, char *string)
{
    int array_size = strlen(string) + 1;
    char *allocated_string = h_alloc_data(heap, array_size * sizeof(char));

    for (int i = 0; i <= array_size; i++) {
        allocated_string[i] = string[i];
    }

    return allocated_string;
}

bool string_contains_character(char *string, char c)
{
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == c) {
            return true;
        }
    }
    return false;
}
