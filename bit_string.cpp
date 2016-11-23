#include "bit_string.hpp"
#include <climits>
#include <stdexcept>
#include <sstream>
#include <iostream>

bit_string::bit_string()
:
    storage_longs{DEFAULT_NUMBER_OF_UINT64S, 0},
    storage_capacity{BITS_PER_UINT64 * DEFAULT_NUMBER_OF_UINT64S},
    size{0}
{}

bit_string::bit_string(const bit_string& to_copy)
:
    size{to_copy.size},
    storage_longs{to_copy.storage_longs},
    storage_capacity{to_copy.storage_capacity}
{}

bit_string& bit_string::operator=(bit_string &&other)
{
    storage_longs = std::move(other.storage_longs);
    storage_capacity = other.storage_capacity;
    size = other.size;
    return *this;
}

void bit_string::append_bit(bool bit)
{
    check_bit_array_capacity(size + 1);
    write_bit_impl(size, bit);
    ++size;
}

size_t bit_string::length() const {
    return size;
}

void bit_string::append_bits_from(const bit_string& bs)
{
    check_bit_array_capacity(size + bs.size);
    size_t other_size = bs.size;
    
    for (size_t i = 0; i != other_size; ++i)
    {
        append_bit(bs.read_bit_impl(i));
    }
}

bool bit_string::read_bit(size_t index) const
{
    check_access_index(index);
    return read_bit_impl(index);
}

void bit_string::remove_last_bit()
{
    if (size == 0)
    {
        throw std::runtime_error{"Removing a bit from empty bit string."};
    }
    
    --size;
}

void bit_string::clear()
{
    size = 0;
}

size_t bit_string::get_number_of_occupied_bytes() const
{
    return size / CHAR_BIT + ((size % CHAR_BIT == 0) ? 0 : 1);
}

std::vector<uint8_t> bit_string::to_byte_array() const
{
    size_t number_of_bytes = get_number_of_occupied_bytes();
    std::vector<uint8_t> ret(number_of_bytes);
    
    for (size_t i = 0; i != number_of_bytes; ++i)
    {
        ret[i] = (uint8_t)((storage_longs[i / sizeof(uint64_t)]
                            >> CHAR_BIT * (i % sizeof(uint64_t))));
    }
    
    return std::move(ret);
}

void bit_string::check_access_index(size_t index) const
{
    if (size == 0)
    {
        throw std::runtime_error{"Accesing an empty bit string."};
    }
    
    if (index >= size)
    {
        std::stringstream ss;
        ss << "The access index is out of range. Index = "
           << index
           << ", length: "
           << size
           << ".";
        
        throw std::runtime_error{ss.str()};
    }
}

bool bit_string::read_bit_impl(size_t index) const
{
    size_t word_index = index / BITS_PER_UINT64;
    size_t bit_index  = index & MODULO_MASK;
    uint64_t mask = 1ULL << bit_index;
    return (storage_longs[word_index] & mask) != 0;
}

void bit_string::write_bit_impl(size_t index, bool bit)
{
    size_t word_index = index / BITS_PER_UINT64;
    size_t bit_index  = index & MODULO_MASK;
    
    if (bit)
    {
        uint64_t mask = (1ULL << bit_index);
        storage_longs[word_index] |= mask;
    }
    else
    {
        uint64_t mask = ~(1ULL << bit_index);
        storage_longs[word_index] &= mask;
    }
}

void bit_string::check_bit_array_capacity(size_t requested_capacity)
{
    if (requested_capacity > storage_capacity)
    {
        size_t requested_words_1 =
            requested_capacity / BITS_PER_UINT64 +
         (((requested_capacity & MODULO_MASK) == 0) ? 0 : 1);
        
        size_t requested_words_2 =
            (3 * storage_capacity / 2) / BITS_PER_UINT64;
        
        size_t selected_requested_words = std::max(requested_words_1,
                                                   requested_words_2);
        
        storage_longs.resize(selected_requested_words);
        storage_capacity = storage_longs.size() * BITS_PER_UINT64;
    }
}
