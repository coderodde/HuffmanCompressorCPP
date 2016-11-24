#include "huffman_serializer.hpp"
#include <algorithm>

const int8_t huffman_serializer::MAGIC[4] = { (int8_t) 0xC0,
                                              (int8_t) 0xDE,
                                              (int8_t) 0x0D,
                                              (int8_t) 0xDE };

const size_t huffman_serializer::BYTES_PER_ENCODER_MAP_ENTRY     = 4;
const size_t huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY = 4;
const size_t huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY       = 4;

static size_t compute_byte_list_size(std::map<int8_t, bit_string> encoder_map,
                                     bit_string& encoded_text)
{
    return sizeof(huffman_serializer::MAGIC)
                  + huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY
                  + huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY
                  + encoder_map.size()
                    * huffman_serializer::BYTES_PER_ENCODER_MAP_ENTRY
                  + encoded_text.get_number_of_occupied_bytes();
}

std::vector<int8_t>&&
huffman_serializer::serialize(std::map<int8_t, bit_string>& encoder_map,
                              bit_string& encoded_text)
{
    std::vector<int8_t> byte_list;
    byte_list.reserve(compute_byte_list_size(encoder_map, encoded_text));
    
    // Emit the file type signature magic:
    for (int8_t magic_byte : huffman_serializer::MAGIC)
    {
        byte_list.push_back(magic_byte);
    }
    
    size_t number_of_code_words = encoder_map.size();
    size_t number_of_text_bits  = encoded_text.length();
    
    // Emit the number of code words.
    byte_list.push_back((int8_t) (number_of_code_words & 0xff));
    byte_list.push_back((int8_t)((number_of_code_words >>= 8) & 0xff));
    byte_list.push_back((int8_t)((number_of_code_words >>= 8) & 0xff));
    byte_list.push_back((int8_t)((number_of_code_words >>= 8) & 0xff));
    
    // Emit the number of bits in the encoded text.
    byte_list.push_back((int8_t) (number_of_text_bits & 0xff));
    byte_list.push_back((int8_t)((number_of_text_bits >>= 8) & 0xff));
    byte_list.push_back((int8_t)((number_of_text_bits >>= 8) & 0xff));
    byte_list.push_back((int8_t)((number_of_text_bits >>= 8) & 0xff));
    
    // Emit the code words:
    for (const auto& entry : encoder_map)
    {
        int8_t character = entry.first;
        int8_t code_word_length = (int8_t) entry.second.length();
        std::vector<int8_t> code_word_bytes = entry.second.to_byte_array();
        
        byte_list.push_back(character);
        byte_list.push_back(code_word_length);
        
        std::copy(code_word_bytes.begin(),
                  code_word_bytes.end(),
                  std::back_inserter(byte_list));
    }
    
    std::vector<int8_t> encoded_text_byte_vector = encoded_text.to_byte_array();
    
    std::copy(encoded_text_byte_vector.begin(),
              encoded_text_byte_vector.end(),
              std::back_inserter(byte_list));
    
    return std::move(byte_list);
}
