#ifndef HUFFMAN_DECODER_HPP
#define HUFFMAN_DECODER_HPP

#include "bit_string.hpp"
#include "huffman_tree.hpp"
#include <vector>

class huffman_decoder {
public:
    std::vector<int8_t> decode(huffman_tree& tree, bit_string& encoded_text);
};

#endif // HUFFMAN_DECODER_HPP
