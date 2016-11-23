#include "huffman_tree.hpp"
#include <cstdint>
#include <map>
#include <vector>

using std::map;
using std::vector;

std::map<uint8_t, float> compute_char_weights(std::vector<uint8_t>& text)
{
    std::map<uint8_t, float> map;
    
    for (auto byte : text)
    {
        map[byte] += 1.0f;
    }
    
    const float text_length_float = (float) text.size();
    
    for (auto& pair_ : map)
    {
        pair_.second /= text_length_float;
    }
    
    return map;
}

