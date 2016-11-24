#ifndef HUFFMAN_ENCODER_HPP
#define HUFFMAN_ENCODER_HPP

#include "bit_string.hpp"
#include <map>
#include <vector>

class huffman_encoder {
public:
    bit_string encode(std::map<int8_t, bit_string>& encoder_map,
                      std::vector<int8_t>& text);
};

#endif // HUFFMAN_ENCODER_HPP
