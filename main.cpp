#include "bit_string.hpp"
#include "byte_weights.hpp"
#include "huffman_decoder.hpp"
#include "huffman_deserializer.hpp"
#include "huffman_encoder.hpp"
#include "huffman_serializer.hpp"
#include "huffman_tree.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

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

static std::string ENCODE_FLAG_SHORT  = "-e";
static std::string ENCODE_FLAG_LONG   = "--encode";
static std::string DECODE_FLAG_SHORT  = "-d";
static std::string DECODE_FLAG_LONG   = "--decode";
static std::string HELP_FLAG_SHORT    = "-h";
static std::string HELP_FLAG_LONG     = "--help";
static std::string VERSION_FLAG_SHORT = "-v";
static std::string VERSION_FLAG_LONG  = "--version";
static std::string ENCODED_FILE_EXTENSION = "het";

static std::string BAD_CMD_FORMAT = "Bad command line format.";

void test_append_bit();
void test_bit_string();
void test_all();

void exec(int argc, const char *argv[]);
void print_help_message(std::string& image_name);
void print_version();
std::string get_base_name(const char *arg1);

void file_write(std::string& file_name, std::vector<int8_t>& data);
std::vector<int8_t> file_read(std::string& file_name);

int main(int argc, const char * argv[])
{
    exec(argc, argv);
    //test_all();
    return 0;
}

void file_write(std::string& file_name, std::vector<int8_t>& data)
{
    std::ofstream file(file_name, std::ios::out | std::ofstream::binary);
    std::size_t size = data.size();
    char* byte_data = new char[size];
    std::copy(data.begin(), data.end(), byte_data);
    file.write(byte_data, size);
    file.close();
}

std::vector<int8_t> file_read(std::string& file_name)
{
    std::ifstream file(file_name, std::ios::in | std::ifstream::binary);
    std::vector<int8_t> ret;
    std::filebuf* pbuf = file.rdbuf();
    std::size_t size = pbuf->pubseekoff(0, file.end, file.in);
    
    pbuf->pubseekpos(0, file.in);
    char* buffer = new char[size];
    pbuf->sgetn(buffer, size);
    
    for (std::size_t i = 0; i != size; ++i)
    {
        ret.push_back((int8_t) buffer[i]);
    }
    
    delete[] buffer;
    file.close();
    return std::move(ret);
}

void do_decode(int argc, const char * argv[])
{
    if (argc != 4)
    {
        throw std::runtime_error{BAD_CMD_FORMAT};
    }
    
    std::string flag = argv[1];
    
    if (flag != DECODE_FLAG_SHORT and flag != DECODE_FLAG_LONG)
    {
        throw std::runtime_error{BAD_CMD_FORMAT};
    }
    
    std::string source_file = argv[2];
    std::string target_file = argv[3];
    
    std::vector<int8_t> encoded_data = file_read(source_file);
    huffman_deserializer deserializer;
    huffman_deserializer::result decode_result =
        deserializer.deserialize(encoded_data);
    
    huffman_tree decoder_tree(decode_result.count_map);
    huffman_decoder decoder;
    std::vector<int8_t> text = decoder.decode(decoder_tree,
                                              decode_result.encoded_text);
    file_write(target_file, text);
}

void do_encode(int argc, const char * argv[])
{
    if (argc != 3)
    {
        throw std::runtime_error{BAD_CMD_FORMAT};
    }
    
    std::string flag = argv[1];
    
    if (flag != ENCODE_FLAG_SHORT and flag != ENCODE_FLAG_LONG)
    {
        throw std::runtime_error{BAD_CMD_FORMAT};
    }
    
    std::string source_file = argv[2];
    std::vector<int8_t> text = file_read(source_file);
    
    std::cout << "File size " << text.size() << std::endl;
    
    std::map<int8_t, uint32_t> count_map = compute_byte_counts(text);
    huffman_tree tree(count_map);
    std::map<int8_t, bit_string> encoder_map = tree.infer_encoder_map();
    huffman_encoder encoder;
    bit_string encoded_text = encoder.encode(encoder_map, text);
    huffman_serializer serializer;
    std::vector<int8_t> encoded_data = serializer.serialize(count_map,
                                                            encoded_text);
    std::string out_file_name = source_file;
    out_file_name += ".";
    out_file_name += ENCODED_FILE_EXTENSION;
    
    file_write(out_file_name, encoded_data);
}

void exec(int argc, const char *argv[])
{
    std::set<std::string> command_line_argument_set;
    std::for_each(argv + 1,
                  argv + argc,
                  [&command_line_argument_set](const char *s) {
                      command_line_argument_set.insert(std::string{s});});
    
    std::string image_name = get_base_name(argv[0]);
    auto args_end = command_line_argument_set.end();
    
    if (command_line_argument_set.find(HELP_FLAG_SHORT) != args_end ||
        command_line_argument_set.find(HELP_FLAG_LONG)  != args_end)
    {
        print_help_message(image_name);
        exit(0);
    }
    
    if (command_line_argument_set.find(VERSION_FLAG_SHORT) != args_end ||
        command_line_argument_set.find(VERSION_FLAG_LONG)  != args_end)
    {
        print_version();
        exit(0);
    }
    
    if (command_line_argument_set.find(DECODE_FLAG_SHORT) != args_end &&
        command_line_argument_set.find(DECODE_FLAG_LONG)  != args_end)
    {
        print_help_message(image_name);
        exit(1);
    }
    
    if (command_line_argument_set.find(ENCODE_FLAG_SHORT) != args_end &&
        command_line_argument_set.find(ENCODE_FLAG_LONG)  != args_end)
    {
        print_help_message(image_name);
        exit(0);
    }
    
    bool decode = false;
    bool encode = false;
    
    if (command_line_argument_set.find(DECODE_FLAG_SHORT) != args_end ||
        command_line_argument_set.find(DECODE_FLAG_LONG)  != args_end)
    {
        decode = true;
    }
    
    if (command_line_argument_set.find(ENCODE_FLAG_SHORT) != args_end ||
        command_line_argument_set.find(ENCODE_FLAG_LONG)  != args_end)
    {
        encode = true;
    }
    
    if ((!decode and !encode) or (decode and encode))
    {
        print_help_message(image_name);
        exit(0);
    }
    
    if (decode)
    {
        do_decode(argc, argv);
    }
    else
    {
        do_encode(argc, argv);
    }
}

std::string get_base_name(const char *cmd_line)
{
    std::string tmp = cmd_line;
 
    if (tmp.empty())
    {
        throw std::runtime_error{"Empty base name string."};
    }
    
    char file_separator;
    
#ifdef _WIN32
    file_separator = '\\';
#else
    file_separator = '/';
#endif
    
    int index = (int) tmp.length() - 1;
    
    for (; index >= 0; --index)
    {
        if (tmp[index] == file_separator)
        {
            std::string ret;
            
            while (++index < tmp.length())
            {
                ret += tmp[index];
            }
            
            return ret;
        }
    }
    
    return tmp;
}

std::string get_indent(size_t len)
{
    std::string ret;
    
    for (size_t i = 0; i != len; ++i)
    {
        ret += ' ';
    }
    
    return ret;
}

void print_help_message(std::string& process_image_name)
{
    std::string preamble = "usage: " + process_image_name + " ";
    size_t preamble_length = preamble.length();
    std::string indent = get_indent(preamble_length);
    cout << preamble;
    
    cout << "[" << HELP_FLAG_SHORT << " | " << HELP_FLAG_LONG << "]\n";
    cout << indent
         << "[" << VERSION_FLAG_SHORT << " | " << VERSION_FLAG_LONG << "]\n";
    cout << indent
         << "[" << ENCODE_FLAG_SHORT << " | " << ENCODE_FLAG_LONG
         << "] FILE\n";
    cout << indent
         << "[" << DECODE_FLAG_SHORT << " | " << DECODE_FLAG_LONG
         << "] FILE_FROM FILE_TO\n";
    
    cout << "Where:" << endl;
    
    cout << HELP_FLAG_SHORT << ", " << HELP_FLAG_LONG
         << "    Print this message and exit.\n";
    cout << VERSION_FLAG_SHORT << ", " << VERSION_FLAG_LONG
         << " Print the version info and exit.\n";
    cout << ENCODE_FLAG_SHORT << ", " << ENCODE_FLAG_LONG
         << "  Encode the text from file.\n";
    cout << DECODE_FLAG_SHORT << ", " << DECODE_FLAG_LONG
         << "  Decode the text from file.\n";
}

void print_version()
{
    cout << "Huffman compressor C++ tool, version 1.6 (Nov 29, 2016)" << endl;
    cout << "By Rodion \"rodde\" Efremov" << endl;
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
