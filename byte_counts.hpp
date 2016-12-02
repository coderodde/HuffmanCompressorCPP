#pragma once

#ifndef BYTE_COUNTS_HPP
#define BYTE_COUNTS_HPP

#include "huffman_tree.hpp"
#include <cstdint>
#include <map>

/***********************************************************************
* Counts relative frequencies of each character represented by a byte. *
***********************************************************************/
std::map<int8_t, uint32_t>
compute_byte_counts(std::vector<int8_t>& text);

#endif // BYTE_WEIGHTS_HPP
