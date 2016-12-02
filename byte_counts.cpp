#include "huffman_tree.hpp"
#include <cstdint>
#include <map>
#include <vector>

using std::map;
using std::vector;

std::map<int8_t, uint32_t> compute_byte_counts(std::vector<int8_t>& text)
{
    std::map<int8_t, uint32_t> map;
    
    for (auto byte : text)
    {
        map[byte] += 1;
    }
    
    return map;
}

