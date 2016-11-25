#include "huffman_deserializer.hpp"
#include "huffman_serializer.hpp"
#include "file_format_error.h"

#include <sstream>
#include <string>

huffman_deserializer::result
huffman_deserializer::deserialize(std::vector<int8_t> &data)
{
    check_signature(data);
    // The number of code words is the same as the number of mappings in the
    // deserialized weight map.
    size_t number_of_code_words = extract_number_of_code_words(data);
    size_t number_of_text_bits  = extract_number_of_encoded_text_bits(data);
    std::map<int8_t, float> weight_map = extract_weight_map(data,
                                                            number_of_code_words);
    bit_string encoded_text = extract_encoded_text(data,
                                                   weight_map,
                                                   number_of_text_bits);
    result ret;
    ret.weight_map   = std::move(weight_map);
    ret.encoded_text = std::move(encoded_text);
    return ret;
}

void huffman_deserializer::check_signature(std::vector<int8_t>& data)
{
    if (data.size() < sizeof(huffman_serializer::MAGIC))
    {
        std::stringstream ss;
        
        ss << "The data is too short to contain "
              "the mandatory signature. Data length: "
           << data.size()
           << ".";
        
        std::string err_msg = ss.str();
        throw file_format_error(err_msg.c_str());
    }
    
    for (size_t i = 0; i != sizeof(huffman_serializer::MAGIC); ++i)
    {
        if (data[i] != huffman_serializer::MAGIC[i])
        {
            throw file_format_error("Bad file type signature.");
        }
    }
}

size_t huffman_deserializer
::extract_number_of_code_words(std::vector<int8_t>& data)
{
    if (data.size() < 8)
    {
        std::stringstream ss;
        ss << "No number of code words in the data. The file is too short: ";
        ss << data.size() << " bytes.";
        std::string err_msg = ss.str();
        throw file_format_error{err_msg.c_str()};
    }
    
    size_t number_of_code_words = 0;
    
    number_of_code_words |= (data[7] << 24);
    number_of_code_words |= (data[6] << 16);
    number_of_code_words |= (data[5] << 8);
    number_of_code_words |= (data[4]);
    
    return number_of_code_words;
}

size_t huffman_deserializer
::extract_number_of_encoded_text_bits(std::vector<int8_t>& data)
{
    if (data.size() < 12)
    {
        std::stringstream ss;
        ss << "No number of encoded text bits. The file is too short: ";
        ss << data.size() << " bytes.";
        std::string err_msg = ss.str();
        throw file_format_error{err_msg.c_str()};
    }
    
    size_t number_of_encoded_text_bits = 0;
    
    number_of_encoded_text_bits |= (data[11] << 24);
    number_of_encoded_text_bits |= (data[10] << 16);
    number_of_encoded_text_bits |= (data[9]  << 8);
    number_of_encoded_text_bits |=  data[8];
    
    return number_of_encoded_text_bits;
}

std::map<int8_t, float> huffman_deserializer::
extract_weight_map(std::vector<int8_t>& data, size_t number_of_code_words)
{
    std::map<int8_t, float> weight_map;
    
    try
    {
        size_t data_byte_index =
            sizeof(huffman_serializer::MAGIC) +
            huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY +
            huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY;
        
        for (size_t i = 0; i != number_of_code_words; ++i)
        {
            int8_t byte = data.at(data_byte_index++);
            
            using wc_type = huffman_serializer::weight_converter;
            wc_type wc;
            
            wc.bytes[0] = data.at(data_byte_index++);
            wc.bytes[1] = data.at(data_byte_index++);
            wc.bytes[2] = data.at(data_byte_index++);
            wc.bytes[3] = data.at(data_byte_index++);
            
            weight_map[byte] = wc.weight;
        }
    }
    catch (std::out_of_range& error)
    {
        std::stringstream ss;
        ss << "The input data is too short in order to recover the encoding "
              "map. "
        << error.what();
        std::string err_msg = ss.str();
        throw file_format_error{err_msg.c_str()};
    }
    
    return weight_map;
}

bit_string huffman_deserializer
::extract_encoded_text(const std::vector<int8_t>& data,
                       const std::map<int8_t, float>& weight_map,
                       const size_t number_of_encoded_text_bits)
{
    size_t omitted_bytes =
        sizeof(huffman_serializer::MAGIC) +
        huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY +
        huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY;
    
    omitted_bytes +=
        weight_map.size() * huffman_serializer::BYTES_PER_WEIGHT_MAP_ENTRY;
    
    bit_string encoded_text;
    size_t current_byte_index = omitted_bytes;
    size_t current_bit_index = 0;
    
    try
    {
        for (size_t bit_index = 0;
             bit_index != number_of_encoded_text_bits;
             bit_index++)
        {
            bool bit =
                (data[current_byte_index] & (1 << current_bit_index)) != 0;
            
            encoded_text.append_bit(bit);
            
            if (++current_bit_index == CHAR_BIT)
            {
                current_bit_index = 0;
                current_byte_index++;
            }
        }
    }
    catch (std::out_of_range& error)
    {
        std::stringstream ss;
        ss << "The input data is too short in order to recover encoded text. "
           << error.what();
        std::string err_msg = ss.str();
        throw file_format_error{err_msg.c_str()};
    }
    
    return encoded_text;
}
