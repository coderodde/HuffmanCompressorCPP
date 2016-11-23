#pragma once

#ifndef BYTE_WEIGHTS_HPP
#define BYTE_WEIGHTS_HPP

#include "huffman_tree.hpp"
#include <cstdint>
#include <map>

/***********************************************************************
* Counts relative frequencies of each character represented by a byte. *
***********************************************************************/
std::map<uint8_t, float>
compute_char_weights(std::vector<uint8_t>& text);

#endif // BYTE_WEIGHTS_HPP
