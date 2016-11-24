#include "huffman_deserializer.hpp"
#include "huffman_serializer.hpp"
#include "file_format_error.h"

#include <sstream>
#include <string>

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
    
}

bit_string huffman_deserializer
::extract_encoded_text(std::vector<int8_t>& data,
                       std::map<int8_t, bit_string>& encoder_map,
                       size_t number_of_encoded_text_bits)
{
    
}
