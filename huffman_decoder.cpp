#include "huffman_decoder.hpp"

std::vector<uint8_t>
huffman_decoder::decode(huffman_tree& tree,
                        bit_string encoded_text)
{
    size_t index = 0;
    size_t bit_string_length = encoded_text.length();
    std::vector<uint8_t> decoded_text;
    
    while (index != bit_string_length)
    {
        uint8_t character = tree.decode_bit_string(index, encoded_text);
        decoded_text.push_back(character);
    }
    
    return decoded_text;
}
