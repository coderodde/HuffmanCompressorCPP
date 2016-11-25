#ifndef HUFFMAN_DESERIALIZER_HPP
#define HUFFMAN_DESERIALIZER_HPP

#include "bit_string.hpp"
#include <map>
#include <cstdint>
#include <vector>

class huffman_deserializer {
public:
    
    struct result {
        bit_string encoded_text;
        std::map<int8_t, float> weight_map;
    };
    
    result deserialize(std::vector<int8_t>& data);
    
private:
    
    // Make sure that the data contains the magic signature:
    void check_signature(std::vector<int8_t>& data);
    
    // Make sure that the data describes the number of code words in the stream
    // and returns that number:
    size_t extract_number_of_code_words(std::vector<int8_t>& data);
    
    // Make sure that the data describes the number of encoded text bits in the
    // stream and returns that number:
    size_t extract_number_of_encoded_text_bits(std::vector<int8_t>& data);
    
    // Extracts the actual encoder map from the stream:
    std::map<int8_t, float>
    extract_weight_map(std::vector<int8_t>& data, size_t number_of_code_words);
    
    // Extracts the actual encoded text from the stream:
    bit_string extract_encoded_text(const std::vector<int8_t>& data,
                                    const std::map<int8_t, float>& weight_map,
                                    const size_t number_of_encoded_text_bits);
};

#endif // HUFFMAN_DESERIALIZER_HPP
