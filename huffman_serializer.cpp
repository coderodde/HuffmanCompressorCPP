#include "huffman_serializer.hpp"
#include <algorithm>

const int8_t huffman_serializer::MAGIC[4] = { (int8_t) 0xC0,
                                              (int8_t) 0xDE,
                                              (int8_t) 0x0D,
                                              (int8_t) 0xDE };

const size_t huffman_serializer::BYTES_PER_WEIGHT_MAP_ENTRY      = 5;
const size_t huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY = 4;
const size_t huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY       = 4;

static size_t compute_byte_list_size(std::map<int8_t, float>& weight_map,
                                     bit_string& encoded_text)
{
    return sizeof(huffman_serializer::MAGIC)
                  + huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY
                  + huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY
                  + weight_map.size()
                    * huffman_serializer::BYTES_PER_WEIGHT_MAP_ENTRY
                  + encoded_text.get_number_of_occupied_bytes();
}

std::vector<int8_t>
huffman_serializer::serialize(std::map<int8_t, float>& weight_map,
                              bit_string& encoded_text)
{
    std::vector<int8_t> byte_list;
    byte_list.reserve(compute_byte_list_size(weight_map, encoded_text));
    
    // Emit the file type signature magic:
    for (int8_t magic_byte : huffman_serializer::MAGIC)
    {
        byte_list.push_back(magic_byte);
    }
    
    union
    {
        size_t num;
        int8_t bytes[4];
    } t;
    
    t.num = weight_map.size();
    
    byte_list.push_back(t.bytes[0]);
    byte_list.push_back(t.bytes[1]);
    byte_list.push_back(t.bytes[2]);
    byte_list.push_back(t.bytes[3]);
    
    t.num = encoded_text.length();
    
    byte_list.push_back(t.bytes[0]);
    byte_list.push_back(t.bytes[1]);
    byte_list.push_back(t.bytes[2]);
    byte_list.push_back(t.bytes[3]);
    
    union
    {
        float weight;
        int8_t bytes[4];
    }
    weight_to_bytes;
    
    // Emit the code words:
    for (const auto& entry : weight_map)
    {
        int8_t byte = entry.first;
        float weight = entry.second;
        weight_to_bytes.weight = weight;
        
        byte_list.push_back(byte);
        byte_list.push_back(weight_to_bytes.bytes[0]);
        byte_list.push_back(weight_to_bytes.bytes[1]);
        byte_list.push_back(weight_to_bytes.bytes[2]);
        byte_list.push_back(weight_to_bytes.bytes[3]);
    }
    
    std::vector<int8_t> encoded_text_byte_vector = encoded_text.to_byte_array();
    
    std::copy(encoded_text_byte_vector.begin(),
              encoded_text_byte_vector.end(),
              std::back_inserter(byte_list));
    
    return byte_list;
}
