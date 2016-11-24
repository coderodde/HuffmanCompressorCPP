#include "bit_string.hpp"
#include "huffman_encoder.hpp"

#include <map>

bit_string huffman_encoder::encode(std::map<int8_t, bit_string>& encoder_map,
                                   std::vector<int8_t>& text)
{
    bit_string output_bit_string;
    size_t text_length = text.size();
    
    for (size_t index = 0; index != text_length; ++index)
    {
        int8_t current_byte = text[index];
        output_bit_string.append_bits_from(encoder_map[current_byte]);
    }
    
    return output_bit_string;
}
