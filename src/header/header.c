#include "header.h"
#include "../gc.h"
#include "../util/binary_util.h"
#include "../bitvector/bitvector.h"
#include "./format_string/format_string.h"
#include "../util/string_util.h"

//Macros används enbart internt
#define FORMAT_STRING_HEADER 10
#define FORWARDING_ADDRESS_HEADER 20
#define STRUCT_LAYOUT_HEADER 40
#define RAW_LAYOUT_HEADER 45

#define FORMAT_STRING 0
#define FORWARDING_ADDRESS 1
#define BITVECTOR_LAYOUT 3

#define AMOUNT_OF_PATTERNS 4

void change_header_type_to(bitvector_t * bitvector, int pattern)
{
    assert(pattern >= 0 && pattern < AMOUNT_OF_PATTERNS);

    bool first_lsb;
    bool second_lsb;
    if (pattern == FORMAT_STRING) {
        first_lsb = false;
        second_lsb = false;
    }
    else if (pattern == FORWARDING_ADDRESS) {
        first_lsb = true;
        second_lsb = false;
    }
    else if (pattern == BITVECTOR_LAYOUT) {
        first_lsb = true;
        second_lsb = true;
    }
    else {
        assert(false && "ERROR: given type for header is invalid");
    }

    bitvector_set_bit(bitvector, 0, first_lsb);
    bitvector_set_bit(bitvector, 1, second_lsb);
}

static void set_bitvector_layout_size(bitvector_t * bitvector, int size)
{
    bitvector_t size_shifted = size << 3;
    for (int i = 3; i <= WORD_SIZE; i++) {
        bitvector_set_bit(bitvector, i, bitvector_read_bit(size_shifted, i));
    }
}

static header_t create_header_with_format_string_pointer(char *format_string)
{
    bitvector_t bitvector = (bitvector_t) format_string;
    change_header_type_to(&bitvector, FORMAT_STRING);
    return (header_t) bitvector;
}

//Oklart när funktionen ska användas 
header_t create_header_with_forwarding_pointer(void *forwarding_pointer)
{
    bitvector_t bitvector = (bitvector_t) forwarding_pointer;
    change_header_type_to(&bitvector, FORWARDING_ADDRESS);
    return (header_t) bitvector;
}

static header_t header_creation_raw(size_t bytes)
{
    assert(bytes > 0 && bytes <= 0x1FFFFFFFFFFFFFFF);
    bitvector_t bitvector = bytes << 3;
    change_header_type_to(&bitvector, BITVECTOR_LAYOUT);
    bitvector_set_bit(&bitvector, 2, false);
    return (header_t) bitvector;
}

static int
required_amount_of_bits_to_represent_struct_with_bitvector_layout(char
                                                                  *format_string)
{
    int result = 0;
    for (int i = 0; i < strlen(format_string); i++) {
        char c = format_string[i];
        int size = format_string_letter_to_size(c);
        if (c == '*' || size == HALF_WORD_SIZE) {
            result += 1;
        }
        else if (size == WORD_SIZE) {
            result += 2;
        }
        else if (size == sizeof(char)) {        // CHEAT: chars otillåtna i structar med pointers
            assert(false
                   &&
                   "ERROR: datatype of size 1 cannot be in struct containing pointers.");
        }
    }
    return result;
}

static bitvector_t create_layout_bitvector_according_to_format_string(char
                                                                      *format_string)
{
    assert(strlen(format_string) != 0);
    bitvector_t bitvector = 0;

    int size_of_layout_bitvector =
        required_amount_of_bits_to_represent_struct_with_bitvector_layout
        (format_string);
    int bitvector_index = BITVECTOR_SIZE - 1;
    for (int i = 0; i < strlen(format_string); i++) {
        char current_character = format_string[i];
        int size = format_string_letter_to_size(current_character);

        bool value_of_bit;
        if (current_character == '*') {
            value_of_bit = true;
        }
        else if (size == HALF_WORD_SIZE) {
            value_of_bit = false;
        }
        else if (size == WORD_SIZE) {
            value_of_bit = false;
            bitvector_set_bit(&bitvector, bitvector_index, value_of_bit);
            bitvector_index--;
        }
        else {
            assert(false
                   &&
                   "ERROR: Character in format string not able to be represented in a layout bitvector");
        }
        bitvector_set_bit(&bitvector, bitvector_index, value_of_bit);
        bitvector_index--;
    }

    set_bitvector_layout_size(&bitvector, size_of_layout_bitvector);

    change_header_type_to(&bitvector, BITVECTOR_LAYOUT);
    bitvector_set_bit(&bitvector, 2, true);
    return bitvector;
}

#define MAX_BITVECTOR_LAYOUT_SIZE 55

header_t header_creation_layout(char *format_string, heap_t * heap)
{
    char format_string_expanded[MAX_ALLOC_SIZE] = { '\0' };
    format_string_expander(format_string, format_string_expanded);

    if (format_string_contains_only_primitive_datatypes(format_string)) {
        int size =
            expanded_format_string_required_space_for_data_alloc
            (format_string_expanded);
        return header_creation_raw(size);
    }

    else if (!string_contains_character(format_string, 'c')
             &&
             required_amount_of_bits_to_represent_struct_with_bitvector_layout
             (format_string_expanded) <= MAX_BITVECTOR_LAYOUT_SIZE) {
        return (header_t)
            create_layout_bitvector_according_to_format_string
            (format_string_expanded);
    }
    else {
        char *string_location = duplicate_string(heap, format_string);
        //      printf("%lu\n", (uint64_t)string_location);
        return create_header_with_format_string_pointer(string_location);
    }
}

static size_t interpret_bitvector(bitvector_t bitvector, int *offset_array)
{
    size_t header_size =
        size_of_struct_allocation((header_t) bitvector) / WORD_SIZE;

    int offset_array_index = 0;
    int struct_offset = 0;
    int bitvector_max_index = BITVECTOR_SIZE - 1;
    for (int i = 0; i < header_size; i++) {
        bool bit_value =
            bitvector_read_bit(bitvector, bitvector_max_index - i);
        if (bit_value != false) {
            if (struct_offset % WORD_SIZE != 0) {
                struct_offset += HALF_WORD_SIZE;
            }

            offset_array[offset_array_index] = struct_offset;
            struct_offset += WORD_SIZE;
            offset_array_index++;
        }
        else if (bit_value == false) {
            struct_offset += HALF_WORD_SIZE;
        }
    }

    return offset_array_index;
}

// HEADER IDENTIFICATION ----------------------------------
static int identify_header(header_t header)
{
    bitvector_t bitvector = (bitvector_t) header;
    bool first_lsb = bitvector_read_bit(bitvector, 0);
    bool second_lsb = bitvector_read_bit(bitvector, 1);

    if (first_lsb == false && second_lsb == false) {
        return FORMAT_STRING_HEADER;
    }
    else if (first_lsb != false && second_lsb == false) {
        return FORWARDING_ADDRESS_HEADER;
    }
    else if (first_lsb != false && second_lsb != false) {
        bool third_lsb = bitvector_read_bit(bitvector, 2);
        if (third_lsb == false) {
            return RAW_LAYOUT_HEADER;
        }
        else {                  //if third_lsb != false
            return STRUCT_LAYOUT_HEADER;
        }
    }
    else {
        assert(false && "ERROR: Cannot identify header");
    }

    return 0;                   //tillagt för "control reaches end of non-void function", nås aldrig
}

bool is_format_string_header(header_t header)
{
    return identify_header(header) == FORMAT_STRING_HEADER;
}

bool is_forwarding_address_header(header_t header)
{
    return identify_header(header) == FORWARDING_ADDRESS_HEADER;
}

bool is_struct_layout_header(header_t header)
{
    return identify_header(header) == STRUCT_LAYOUT_HEADER;
}

bool is_raw_layout_header(header_t header)
{
    return identify_header(header) == RAW_LAYOUT_HEADER;
}

//-------------------

size_t header_interpretation(header_t header, int *offset_array)
{
    if (is_format_string_header(header)) {
        char *format_string = (char *) header;
        size_t amount_of_pointers =
            interpret_format_string(format_string, offset_array);
        return amount_of_pointers;
    }
    else if (is_struct_layout_header(header)) {
        bitvector_t bitvector = (bitvector_t) header;
        size_t amount_of_pointers =
            interpret_bitvector(bitvector, offset_array);
        return amount_of_pointers;
    }
    else {
        assert(false
               &&
               "ERROR: header cannot be interpreted because it's neither a format string or a bitvector");
    }
}

size_t size_of_raw_allocation(header_t header)
{
    assert(is_raw_layout_header(header));
    return (uint64_t) header >> (uint64_t) 3;
}

#define Extract_field_count(header) (((uint64_t) header << (uint64_t) 55) >> (uint64_t) 58)

size_t size_of_struct_allocation(header_t header)
{
    bool is_format_string = is_format_string_header(header);
    bool is_struct_layout = is_struct_layout_header(header);
    assert(is_format_string || is_struct_layout);

    if (is_format_string) {
        char *format_string = (char *) header;
        char buffer[MAX_ALLOC_SIZE] = { '\0' };
        format_string_expander(format_string, buffer);
        return strlen(buffer) * WORD_SIZE;
    }
    else {                      //if is_struct_layout

        uint64_t amount_of_fields = Extract_field_count(header);
        return amount_of_fields * WORD_SIZE;    //Uses less space than this but not more
    }
}

void *get_forwarding_pointer_from_header(header_t header)
{
    assert(is_forwarding_address_header(header));

    bitvector_t bitvector = (bitvector_t) header;
    bitvector_set_bit(&bitvector, 0, false);

    return (void *) bitvector;
}

//gör inget förutom validation men är ändå bra för abstraktionens skull
char *get_format_string_from_header(header_t header)
{
    assert(is_format_string_header(header));

    return (char *) header;
}
