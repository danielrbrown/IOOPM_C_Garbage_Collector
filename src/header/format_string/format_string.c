#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "../../util/string_util.h"
#include "format_string.h"

#define THREE_DIGIT_STRING 4

//------ helper functions
static bool format_string_char_is_valid(char letter)
{
    char *valid_chars = "*cilfd";
    for (int i = 0; i < strlen(valid_chars); i++) {
        if (valid_chars[i] == letter) {
            return true;
        }
    }
    // if letter matches none of the valid ones
    return false;
}

size_t format_string_letter_to_size(char letter)
{
    if (letter == 'c') {
        return sizeof(char);
    }
    else if (letter == 'i') {
        return sizeof(int);
    }
    else if (letter == 'l') {
        return sizeof(long);
    }
    else if (letter == 'f') {
        return sizeof(float);
    }
    else if (letter == 'd') {
        return sizeof(double);
    }
    else if (letter == '*') {
        return sizeof(void *);
    }
    else {
        assert(false &&
               "ERROR: given letter from format string doesn't represent any data type");
    }
}

static bool expanded_format_string_is_valid(char *format_string)
{
    if (format_string_contains_only_numbers(format_string)) {
        return true;
    }

    for (int i = 0; i < strlen(format_string); i++) {
        char current_char = format_string[i];
        bool char_is_number = is_ascii_number(current_char);
        if (!char_is_number && !format_string_char_is_valid(current_char)) {
            return false;
        }
    }
    return true;
}

//---------------------------------

void format_string_expander(char *format_string, char buffer[])
{
    if (format_string_contains_only_numbers(format_string)) {
        int number = atoi(format_string);
        for (int i = 0; i < number; i++) {
            buffer[i] = 'c';
        }
        buffer[number] = '\0';
        assert(expanded_format_string_is_valid(buffer)
               && "ERROR: format string is invalid");
        return;
    }

    int buffer_index = 0;
    for (int i = 0; i < strlen(format_string); i++) {
        char current_character = format_string[i];

        if (!is_ascii_number(current_character)) {
            buffer[buffer_index] = format_string[i];
            buffer_index++;
        }

        else {
            char number_string[THREE_DIGIT_STRING] = { '\0' };
            int number_string_index = 0;
            while (is_ascii_number(format_string[i])) {
                number_string[number_string_index] = format_string[i];
                i++;
                number_string_index++;
            }
            assert(number_string_index < THREE_DIGIT_STRING
                   && "ERROR: Coefficient in format string is too large");

            int number_string_as_int = atoi(number_string);
            char character_to_insert = format_string[i];
            while (number_string_as_int != 0) {
                buffer[buffer_index] = character_to_insert;
                buffer_index++;
                number_string_as_int--;
            }
        }
    }

    buffer[buffer_index] = '\0';
    assert(expanded_format_string_is_valid(buffer)
           && "ERROR: format string is invalid");
}

bool format_string_contains_only_primitive_datatypes(char *format_string)
{
    for (int i = 0; i < strlen(format_string); i++) {
        if (format_string[i] == '*') {
            return false;
        }
    }

    return true;
}

bool format_string_contains_only_numbers(char *format_string)
{
    for (int i = 0; i < strlen(format_string); i++) {
        char current_char = format_string[i];
        if (!is_ascii_number(current_char)) {
            return false;
        }
    }
    return true;
}

static bool expanded_format_string_is_array(char *format_string)
{
    if (strlen(format_string) == 1) {
        return false;
    }
    char first_char = format_string[0];
    for (int i = 1; i < strlen(format_string); i++) {
        if (first_char != format_string[i]) {
            return false;
        }
    }
    return true;
}

int expanded_format_string_required_space_for_data_alloc(char *format_string)
{
    if (expanded_format_string_is_array(format_string)) {
        int size =
            strlen(format_string) *
            format_string_letter_to_size(format_string[0]);
        while (size % WORD_SIZE != 0) {
            size++;
        }
        return size;
    }

    int result = 0;

    int padding_counter = WORD_SIZE;
    for (int i = 0; i < strlen(format_string); i++) {
        char c = format_string[i];
        int field_size = format_string_letter_to_size(c);

        if (field_size <= padding_counter) {
            result += field_size;
            padding_counter -= field_size;
        }
        else {
            result += padding_counter;
            result += field_size;
            padding_counter = WORD_SIZE - field_size;
        }
    }

    if (padding_counter != 0 && padding_counter != WORD_SIZE) {
        result += padding_counter;
    }
    return result;
}

size_t interpret_format_string(char *format_string, int *offset_array)
{
    char format_string_expanded[MAX_ALLOC_SIZE] = { '\0' };
    format_string_expander(format_string, format_string_expanded);

    int offset_array_index = 0;
    int offset = 0;
    int padding = WORD_SIZE;
    for (int i = 0; i < strlen(format_string_expanded); i++) {
        char c = format_string_expanded[i];
        if (c == '*') {
            offset_array[offset_array_index] = offset;
            offset_array_index++;
            offset += WORD_SIZE;
            padding = WORD_SIZE;
        }
        else {
            int size = format_string_letter_to_size(c);
            if (size > padding) {
                offset += padding;
                padding = WORD_SIZE;
            }
            else {
                offset += size;
                padding -= size;
            }
        }
    }
    return offset_array_index;
}
