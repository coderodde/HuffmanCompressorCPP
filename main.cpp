#include "bit_string.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#define ASSERT(C) if (!(C)) report(__FILE__, __LINE__)

void report(char* file, size_t line)
{
    cerr << "File: " << file << ", line: " << line << endl;
    exit(1);
}

void test_append_bit();
void test_bit_string();
void test_all();

int main(int argc, const char * argv[])
{
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
    catch (std::runtime_error& err)
    {
        
    }
    
    try
    {
        b.read_bit(-1); ASSERT(false);
    }
    catch (std::runtime_error& err)
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
}

void test_all()
{
    test_bit_string();
}
