#ifndef HUFFMAN_SERIALIZER_HPP
#define HUFFMAN_SERIALIZER_HPP

#include "bit_string.hpp"
#include <cstdint>
#include <cstdlib>
#include <map>
#include <vector>

class huffman_serializer {
public:
    static const int8_t MAGIC[4];/* = { (int8_t) 0xC0,
                                        (int8_t) 0xDE,
                                        (int8_t) 0x0D,
                                        (int8_t) 0xDE };
    */
    static const size_t BYTES_PER_ENCODER_MAP_ENTRY;//     = 4;
    static const size_t BYTES_PER_CODE_WORD_COUNT_ENTRY;// = 4;
    static const size_t BYTES_PER_BIT_COUNT_ENTRY;//      = 4;
    
    std::vector<int8_t>&& serialize(std::map<int8_t, bit_string>& encoder_map,
                                    bit_string& encoded_text);
};

#endif // HUFFMAN_SERIALIZER_HPP
