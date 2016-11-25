#ifndef HUFFMAN_SERIALIZER_HPP
#define HUFFMAN_SERIALIZER_HPP

#include "bit_string.hpp"
#include <cstdint>
#include <cstdlib>
#include <map>
#include <vector>

class huffman_serializer {
public:
    
    static union weight_converter
    {
        float weight;
        int8_t bytes[4];
    }
    weight_to_bytes;
    
    static const int8_t MAGIC[4];
    static const size_t BYTES_PER_WEIGHT_MAP_ENTRY;
    static const size_t BYTES_PER_CODE_WORD_COUNT_ENTRY;
    static const size_t BYTES_PER_BIT_COUNT_ENTRY;
    
    std::vector<int8_t>&& serialize(std::map<int8_t, float>& weight_map,
                                    bit_string& encoded_text);
};

#endif // HUFFMAN_SERIALIZER_HPP
