#include "bitvector.h"

#define BITVECTOR_ALL_ONES 0xffffffffffffffff   // The largest binary number that can fit in a bitvector

bitvector_t bitvector_create(uint64_t initial_value)
{
    if (initial_value < 0 || initial_value > BITVECTOR_ALL_ONES) {
        assert(false
               &&
               "ERROR: Initialisation value must be between 0 and 0xffffffffffffffff");
    }
    return (bitvector_t) initial_value;
}

void bitvector_array_create(bitvector_t * bitvector_array,
                            uint64_t * initial_values, size_t length)
{
    for (int i = 0; i < length; i++) {
        bitvector_array[i] = initial_values[i];
    }
}

void bitvector_set_bit(bitvector_t * bitvector, int index, bool value)
{
    assert(index >= 0 && index < BITVECTOR_SIZE);
    if (value) {
        *bitvector |= ((uint64_t) 1 << (index));
    }
    else {
        *bitvector &= ~((uint64_t) 1 << (index));
    }
}

void bitvector_array_set_bit(bitvector_t * bitvector_array, int index,
                             bool value)
{
    int array_index = index / BITVECTOR_SIZE;
    int bit_index = index % BITVECTOR_SIZE;

    bitvector_set_bit(&bitvector_array[array_index], bit_index, value);
}

bool bitvector_read_bit(bitvector_t bitvector, int index)
{
    assert(index >= 0 && index < BITVECTOR_SIZE);
    return (bitvector & ((uint64_t) 1) << index);
}

bool bitvector_array_read_bit(bitvector_t * bitvector_array, int index)
{
    int array_index = index / BITVECTOR_SIZE;
    int bit_index = index % BITVECTOR_SIZE;

    return bitvector_read_bit(bitvector_array[array_index], bit_index);
}

size_t bitvector_count_ones(bitvector_t bitvector)
{
    size_t count = 0;
    for (int i = 0; i < BITVECTOR_SIZE; i++) {
        if (bitvector & (uint64_t) 1) {
            count++;
        }
        bitvector >>= (uint64_t) 1;
    }
    assert(count >= 0 && count < BITVECTOR_SIZE + 1);
    return count;
}

size_t bitvector_array_count_ones(bitvector_t * bitvector_array,
                                  size_t array_length)
{
    size_t count = 0;
    for (int i = 0; i < array_length; i++) {
        count += bitvector_count_ones(bitvector_array[i]);
    }
    return count;
}
