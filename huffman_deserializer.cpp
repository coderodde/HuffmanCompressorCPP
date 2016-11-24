#include "huffman_deserializer.hpp"
#include "huffman_serializer.hpp"
#include "file_format_error.h"

#include <sstream>
#include <string>

huffman_deserializer::result
huffman_deserializer::deserialize(std::vector<int8_t> &data)
{
    check_signature(data);
    size_t number_of_code_words = extract_number_of_code_words(data);
    size_t number_of_text_bits  = extract_number_of_encoded_text_bits(data);
    std::map<int8_t, bit_string> encoder_map =
        extract_encoder_map(data,
                            number_of_code_words);
    
    bit_string encoded_text = extract_encoded_text(data,
                                                   encoder_map,
                                                   number_of_text_bits);
    result ret;
    ret.encoder_map  = std::move(encoder_map);
    ret.encoded_text = std::move(encoded_text);
    return ret;
}

void huffman_deserializer
::check_signature(std::vector<int8_t>& data)
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

std::map<int8_t, bit_string> huffman_deserializer::
extract_encoder_map(std::vector<int8_t>& data, size_t number_of_code_words)
{
    std::map<int8_t, bit_string> encoder_map;
    
    try
    {
        size_t data_byte_index =
            sizeof(huffman_serializer::MAGIC) +
            huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY +
            huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY;
        
        for (size_t i = 0; i != number_of_code_words; ++i)
        {
            int8_t character = data[data_byte_index++];
            size_t code_word_length = data[data_byte_index++];
            size_t bit_index = 0;
            bit_string code_word_bits;
            
            for (size_t code_word_bit_index = 0;
                 code_word_bit_index != code_word_length;
                 code_word_bit_index++)
            {
                int8_t current_byte = data[data_byte_index];
                bool bit = (current_byte & (1 << bit_index)) != 0;
                code_word_bits.append_bit(bit);
                
                if (++bit_index == CHAR_BIT)
                {
                    bit_index = 0;
                    data_byte_index++;
                }
            }
            
            encoder_map[character] = std::move(code_word_bits);
            
            if (bit_index != 0)
            {
                data_byte_index++;
            }
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
    
    return encoder_map;
}

bit_string huffman_deserializer
::extract_encoded_text(std::vector<int8_t>& data,
                       std::map<int8_t, bit_string>& encoder_map,
                       size_t number_of_encoded_text_bits)
{
    size_t omitted_bytes =
        sizeof(huffman_serializer::MAGIC) +
        huffman_serializer::BYTES_PER_BIT_COUNT_ENTRY +
        huffman_serializer::BYTES_PER_CODE_WORD_COUNT_ENTRY;
    
    for (const auto& p : encoder_map)
    {
        omitted_bytes += 2 + p.second.get_number_of_occupied_bytes();
    }
    
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
