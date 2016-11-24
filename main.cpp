#include "bit_string.hpp"
#include "byte_weights.hpp"
#include "huffman_tree.hpp"

#include <iostream>
#include <map>
#include <stdexcept>

using std::cout;
using std::cerr;
using std::endl;
using std::runtime_error;

#define ASSERT(C) if (!(C)) report(__FILE__, __LINE__)

void report(const char* file, size_t line)
{
    cerr << "File: " << file << ", line: " << line << endl;
    exit(1);
}

void test_append_bit();
void test_bit_string();
void test_all();

int main(int argc, const char * argv[])
{
    std::vector<int8_t> text = {(int8_t) 'h', (int8_t) 'w', (int8_t) 0xb9 };
    //{'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
    
    std::map<int8_t, float> weight_map = compute_char_weights(text);
    
    for (auto& p : weight_map)
    {
        cout << p.first << " " << p.second << endl;
    }
    
    exit(0);
    
    huffman_tree tree(weight_map);
    std::map<int8_t, bit_string> encoder_map = tree.infer_encoder_map();
    
    for (auto p : encoder_map)
    {
        cout << (char) p.first << ": " << p.second << endl;
    }
    
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
        ASSERT(0xaa == bytes[i]);
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

void test_all()
{
    test_bit_string();
    cout << "All tests passed." << endl;
}
