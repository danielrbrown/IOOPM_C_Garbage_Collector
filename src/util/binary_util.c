#include "binary_util.h"

//Might be better to move this function to another module 
static uint64_t myPow(uint64_t base, int exponent)
{
    uint64_t result = 1;

    for (int i = 0; i < exponent; i++) {
        result *= base;
    }

    return result;
}

uint64_t binary_string_to_uint64(char *binary_string)
{
    assert(strlen(binary_string) == sizeof(uint64_t) * 8);
    uint64_t result = 0;
    for (int i = 0; i < strlen(binary_string); i++) {
        char current_character = binary_string[i];
        if (current_character == '1') {
            result += myPow(2, strlen(binary_string) - i - 1);
        }
        else if (current_character == '0') {
            continue;
        }
        else {
            assert(0);
        }
    }
    return result;
}

#define GROUPING_SIZE_OF_PRINTED_BINARY 8
void _print_uint64_as_binary(uint64_t n, int index)
{
    if (index != 0) {
        index -= 1;
        if (n & (uint64_t) 1) {
            n >>= (uint64_t) 1;
            _print_uint64_as_binary(n, index);
            if ((index % GROUPING_SIZE_OF_PRINTED_BINARY) == 0) {
                printf(" ");
            }
            printf("1");

        }
        else {
            n >>= (uint64_t) 1;
            _print_uint64_as_binary(n, index);
            if ((index % GROUPING_SIZE_OF_PRINTED_BINARY) == 0) {
                printf(" ");
            }
            printf("0");

        }
    }
}

void print_uint64_as_binary(uint64_t n)
{
    _print_uint64_as_binary(n, sizeof(uint64_t) * 8);
    printf("\n");
}


void print_heap(uint64_t * starting_adress, int words)
{
    for (int i = 0; i < words; i++) {
        print_uint64_as_binary(starting_adress[i]);
    }
}
