#include "bit_string.hpp"
#include "byte_weights.hpp"
#include "huffman_decoder.hpp"
#include "huffman_deserializer.hpp"
#include "huffman_encoder.hpp"
#include "huffman_serializer.hpp"
#include "huffman_tree.hpp"

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <stdexcept>
#include <cstdint>

using std::cout;
using std::cerr;
using std::endl;
using std::equal;
using std::runtime_error;

#define ASSERT(C) if (!(C)) report(#C, __FILE__, __LINE__)

void report(const char* condition, const char* file, size_t line)
{
    cerr << "The condition \""
         << condition
         << "\" failed in file \""
         << file
         << "\", line: "
         << line
         << "."
         << endl;
    exit(1);
}

void test_append_bit();
void test_bit_string();
void test_all();

int main(int argc, const char * argv[])
{
//    std::vector<int8_t> text = {(int8_t) 'h', (int8_t) 'w', (int8_t) 0xb9 };
//    //{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
//    
//    std::map<int8_t, float> weight_map = compute_char_weights(text);
//    
//    for (auto& p : weight_map)
//    {
//        cout << p.first << " " << p.second << endl;
//    }
//    
//    exit(0);
//    
//    huffman_tree tree(weight_map);
//    std::map<int8_t, bit_string> encoder_map = tree.infer_encoder_map();
//    
//    for (auto p : encoder_map)
//    {
//        cout << (char) p.first << ": " << p.second << endl;
//    }
    
    std::vector<int8_t> text = {1, 2, 3, 1, 2, 1};
    
    std::map<int8_t, uint32_t> cmap = compute_byte_counts(text);
    huffman_tree tree(cmap);
    auto encoder_map = tree.infer_encoder_map();
    huffman_encoder encoder;
    bit_string b = encoder.encode(encoder_map, text);
    cout << b << endl;
    
    test_all();
    return 0;
}

void test_append_bit()
{
    bit_string b;
    
    ASSERT(b.length() == 0);
    
    for (int i = 0; i < 100; ++i)
    {
        ASSERT(b.length() == i);
        b.append_bit(false);
    }
    
    for (int i = 0; i < 30; ++i)
    {
        ASSERT(b.length() == 100 + i);
        b.append_bit(true);
    }
    
    ASSERT(b.length() == 130);
    
    for (int i = 0; i < 100; ++i)
    {
        ASSERT(b.read_bit(i) == false);
    }
    
    for (int i = 100; i < 130; ++i)
    {
        ASSERT(b.read_bit(i) == true);
    }
}

void test_append_bits_from()
{
    bit_string b;
    bit_string c;
    
    for (int i = 0; i < 200; ++i)
    {
        b.append_bit(false);
    }
    
    for (int i = 0; i < 100; ++i)
    {
        c.append_bit(true);
    }
    
    ASSERT(b.length() == 200);
    ASSERT(c.length() == 100);
    
    b.append_bits_from(c);
    
    ASSERT(b.length() == 300);
    ASSERT(c.length() == 100);
    
    for (int i = 0; i < 200; ++i)
    {
        ASSERT(b.read_bit(i) == false);
    }
    
    for (int i = 200; i < 300; ++i)
    {
        ASSERT(b.read_bit(i) == true);
    }
}

void test_read_bit()
{
    bit_string b;
    b.append_bit(true);
    b.append_bit(false);
    b.append_bit(false);
    b.append_bit(true);
    b.append_bit(false);
    
    ASSERT(b.length() == 5);
    
    ASSERT(b.read_bit(0) == true);
    ASSERT(b.read_bit(3) == true);
    
    ASSERT(b.read_bit(1) == false);
    ASSERT(b.read_bit(2) == false);
    ASSERT(b.read_bit(4) == false);
}

void test_read_bit_bad_index_throws()
{
    bit_string b;
    b.append_bit(true);
    
    try
    {
        b.read_bit(1); ASSERT(false);
    }
    catch (runtime_error& err)
    {
        
    }
    
    try
    {
        b.read_bit(-1); ASSERT(false);
    }
    catch (runtime_error& err)
    {
        
    }
}

void test_read_bit_throws_on_empty_string()
{
    bit_string b;
    
    try
    {
        b.read_bit(0); ASSERT(false);
    }
    catch (std::runtime_error& err)
    {
        
    }
}

void test_remove_last_bit()
{
    bit_string b;
    
    b.append_bit(true);
    b.append_bit(true);
    b.append_bit(false);
    b.append_bit(true);
    b.append_bit(false);
    
    ASSERT(b.read_bit(0) == true);
    ASSERT(b.read_bit(1) == true);
    ASSERT(b.read_bit(2) == false);
    ASSERT(b.read_bit(3) == true);
    ASSERT(b.read_bit(4) == false);
    
    for (int i = 5; i > 0; --i)
    {
        ASSERT(b.length() == i);
        b.remove_last_bit();
        ASSERT(b.length() == i - 1);
    }
}

void test_remove_last_bit_throws_on_empty_string()
{
    bit_string b;
    
    try
    {
        b.remove_last_bit(); ASSERT(false);
    }
    catch (std::runtime_error& err)
    {
        
    }
}

void test_bit_string_clear()
{
    bit_string b;
    
    for (int i = 0; i < 1000; ++i)
    {
        ASSERT(b.length() == i);
        b.append_bit(true);
        ASSERT(b.length() == i + 1);
    }
    
    b.clear();
    ASSERT(b.length() == 0);
}

void test_bit_string_get_number_of_occupied_bytes()
{
    bit_string b;
    ASSERT(0 == b.get_number_of_occupied_bytes());
    
    for (int i = 0; i < 100; ++i)
    {
        ASSERT(b.get_number_of_occupied_bytes() == i);
        
        for (int j = 0; j < 8; ++j)
        {
            b.append_bit(true);
            ASSERT(i + 1 == b.get_number_of_occupied_bytes());
        }
        
        ASSERT(i + 1 == b.get_number_of_occupied_bytes());
    }
}

void test_bit_string_to_byte_array()
{
    bit_string b;
    
    for (int i = 0; i < 40; ++i)
    {
        b.append_bit(i % 2 == 1);
    }
    
    for (int i = 0; i < 80; ++i)
    {
        b.append_bit(i % 2 == 0);
    }
    
    std::vector<int8_t> bytes{b.to_byte_array()};
    
    for (int i = 0; i < 5; ++i)
    {
        ASSERT(0xaa == (uint8_t) bytes[i]);
    }
    
    for (int i = 5; i < 15; ++i)
    {
        ASSERT(0x55 == bytes[i]);
    }
}

void test_bit_string()
{
    test_append_bit();
    test_append_bits_from();
    test_read_bit();
    test_read_bit_bad_index_throws();
    test_read_bit_throws_on_empty_string();
    test_remove_last_bit();
    test_remove_last_bit_throws_on_empty_string();
    test_bit_string_clear();
    test_bit_string_get_number_of_occupied_bytes();
    test_bit_string_to_byte_array();
}

std::vector<int8_t> random_text()
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<size_t> uniform_dist_length(0, 10 * 100);
    std::uniform_int_distribution<int8_t>
        uniform_dist(std::numeric_limits<int8_t>::min(),
                     std::numeric_limits<int8_t>::max());
    
    size_t len = uniform_dist_length(engine);
    std::vector<int8_t> ret;
    
    for (size_t i = 0; i != len; ++i)
    {
        ret.push_back(uniform_dist(engine));
    }
    
    return ret;
}

void test_brute_force()
{
    std::vector<int8_t> text = random_text();
    std::map<int8_t, uint32_t> count_map = compute_byte_counts(text);
    
    huffman_tree tree(count_map);
    
    std::map<int8_t, bit_string> encoder_map = tree.infer_encoder_map();
    
    huffman_encoder encoder;
    
    bit_string text_bit_string = encoder.encode(encoder_map, text);
    
    huffman_serializer serializer;
    
    std::vector<int8_t> encoded_data = serializer.serialize(count_map,
                                                            text_bit_string);
    huffman_deserializer deserializer;
    huffman_deserializer::result hdr = deserializer.deserialize(encoded_data);
    
    huffman_tree decoder_tree(hdr.count_map);
    huffman_decoder decoder;
    
    ASSERT(hdr.count_map.size() == count_map.size());
    
    for (auto& e : hdr.count_map)
    {
        auto iter = count_map.find(e.first);
        ASSERT(iter != count_map.end());
        ASSERT(e.second == iter->second);
    }
    
    ASSERT(text_bit_string.length() == hdr.encoded_text.length());
    
    std::vector<int8_t> recovered_text = decoder.decode(decoder_tree,
                                                        hdr.encoded_text);
    ASSERT(text.size() == recovered_text.size());
    ASSERT(std::equal(text.begin(), text.end(), recovered_text.begin()));
}

void test_simple_algorithm()
{
    std::string text = "hello world";
    std::vector<int8_t> text_vector;
    
    for (char c : text)
    {
        text_vector.push_back((int8_t) c);
    }
    
    std::map<int8_t, uint32_t> count_map = compute_byte_counts(text_vector);
    
    huffman_tree tree(count_map);
    
    std::map<int8_t, bit_string> encoder_map = tree.infer_encoder_map();
    
    huffman_encoder encoder;
    
    bit_string text_bit_string = encoder.encode(encoder_map, text_vector);
    
    huffman_serializer serializer;
    
    std::vector<int8_t> encoded_text = serializer.serialize(count_map,
                                                            text_bit_string);
    
    huffman_deserializer deserializer;
    
    huffman_deserializer::result hdr = deserializer.deserialize(encoded_text);
    
    huffman_tree decoder_tree(hdr.count_map);
    huffman_decoder decoder;
    
    std::vector<int8_t> recovered_text = decoder.decode(decoder_tree,
                                                        hdr.encoded_text);
    ASSERT(text.size() == recovered_text.size());
    ASSERT(std::equal(text.begin(), text.end(), recovered_text.begin()));
}

void test_one_byte_text()
{
    std::vector<int8_t> text = { 0x01, 0x01 };
    std::map<int8_t, uint32_t> count_map = compute_byte_counts(text);
    huffman_tree tree(count_map);
    std::map<int8_t, bit_string> encoder_map = tree.infer_encoder_map();
    huffman_encoder encoder;
    bit_string text_bit_string = encoder.encode(encoder_map, text);
    huffman_serializer serializer;
    std::vector<int8_t> serialized_text = serializer.serialize(count_map,
                                                               text_bit_string);
    huffman_deserializer deserializer;
    huffman_deserializer::result hdr =
        deserializer.deserialize(serialized_text);
    
    huffman_tree decoder_tree(hdr.count_map);
    
    huffman_decoder decoder;
    
    std::vector<int8_t> recovered_text = decoder.decode(decoder_tree,
                                                        hdr.encoded_text);
    ASSERT(recovered_text.size() == 2);
    ASSERT(recovered_text[0] = 0x1);
    ASSERT(recovered_text[1] = 0x1);
}

void test_algorithms()
{
    test_simple_algorithm();
    test_one_byte_text();
    
    for (int iter = 0; iter != 100; ++iter)
    {
        test_brute_force();
    }
}

void test_all()
{
    test_bit_string();
    test_algorithms();
    cout << "All tests passed." << endl;
}
