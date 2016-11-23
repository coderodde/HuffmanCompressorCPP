#pragma once

#ifndef CODERODDE_BIT_STRING
#define CODERODDE_BIT_STRING

#include <cstdint>
#include <iostream>
#include <vector>

class bit_string {
public:
    
    constexpr static size_t DEFAULT_NUMBER_OF_UINT64S = 2;
    constexpr static size_t MODULO_MASK = 0x3F;
    constexpr static size_t BITS_PER_UINT64 = 64;
    
    /**********************************
    * Constructs an empty bit string. *
    **********************************/
    explicit bit_string();
    
    /***********************************
    * Copy constructs this bit string. *
    ***********************************/
    explicit bit_string(const bit_string& to_copy);
    
    /********************************
    * The move assignment operator. *
    ********************************/
    bit_string& operator=(bit_string&& other);
    
    /************************
    * The move constructor. *
    ************************/
    bit_string(bit_string&& other);
    
    /************************************
    * Appends 'bit' to this bit string. *
    ************************************/
    void append_bit(bool bit);
    
    /*************************************************
    * Returns the number of bits in this bit string. *
    *************************************************/
    size_t length() const;
    
    /***********************************************
    * Appends 'bs' to the tail of this bit string. *
    ***********************************************/
    void append_bits_from(const bit_string& bs);
    
    /********************************
    * Reads a bit at index 'index'. *
    ********************************/
    bool read_bit(size_t index) const;
    
    /*********************************************
    * Removes the last bit from this bit string. *
    *********************************************/
    void remove_last_bit();
    
    /*******************************************
    * Removes all the bits in this bit string. *
    *******************************************/
    void clear();
    
    /***************************************************************************
    * Returns the number of bytes it takes to accommodate all the bits in this *
    * bit string.                                                              *
    ***************************************************************************/
    size_t get_number_of_occupied_bytes() const;
    
    /***********************************************************************
    * Returns an array of bytes holding all the bits from this bit string. *
    ***********************************************************************/
    std::vector<uint8_t> to_byte_array() const;
    
    friend std::ostream& operator<<(std::ostream& out, bit_string& b)
    {
        for (size_t i = 0; i != b.length(); ++i)
        {
            out << (b.read_bit(i) ? '1' : '0');
        }
        
        return out;
    }
    
private:
    std::vector<uint64_t> storage_longs;
    size_t storage_capacity;
    size_t size;
    
    // Makes sure that the index is within the range.
    void check_access_index(size_t index) const;
    
    // An implementation of the reading operation. Does not check the index.
    bool read_bit_impl(size_t index) const;
    
    // An implementation of the writing operation. Does not check the index.
    void write_bit_impl(size_t index, bool bit);
    
    // Makes sure that this bit string can fit 'requested_capacity' bits.
    void check_bit_array_capacity(size_t requested_capacity);
};

#endif // CODERODDE_BIT_STRING
