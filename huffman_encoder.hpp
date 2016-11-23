#ifndef HUFFMAN_ENCODER_HPP
#define HUFFMAN_ENCODER_HPP

#include "bit_string.hpp"
#include <map>
#include <vector>

class huffman_encoder {
public:
    bit_string encode(std::map<uint8_t, bit_string> encoder_map,
                      std::vector<uint8_t> text);
};

#endif // HUFFMAN_ENCODER_HPP
