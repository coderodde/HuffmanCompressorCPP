#include "bit_string.hpp"
#include "huffman_tree.hpp"
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

huffman_tree::huffman_tree(std::map<int8_t, uint32_t>& count_map)
{
    if (count_map.empty())
    {
        std::stringstream ss;
        ss << "Compressor requires a non-empty text.";
        throw std::runtime_error{ss.str()};
    }
    
    std::priority_queue<huffman_tree_node*,
                        std::vector<huffman_tree_node*>,
                        huffman_tree::huffman_tree_node_comparator> queue;
    
    std::for_each(count_map.cbegin(),
                  count_map.cend(),
                  [&queue](std::pair<int8_t, uint32_t> p) {
                      queue.push(new huffman_tree_node(p.first,
                                                       p.second,
                                                       true));
                  });
    
    while (queue.size() > 1)
    {
        huffman_tree_node* node1 = queue.top(); queue.pop();
        huffman_tree_node* node2 = queue.top(); queue.pop();
        queue.push(merge(node1, node2));
    }
    
    root = queue.top(); queue.pop();
}

void huffman_tree::recursive_node_delete(huffman_tree::huffman_tree_node* node)
{
    if (node == nullptr)
    {
        return;
    }
    
    recursive_node_delete(node->left);
    recursive_node_delete(node->right);
    
    delete node;
}

huffman_tree::~huffman_tree()
{
    recursive_node_delete(root);
}

std::map<int8_t, bit_string> huffman_tree::infer_encoder_map()
{
    std::map<int8_t, bit_string> map;
    
    if (root->is_leaf)
    {
        root->is_leaf = false;
        root->left = new huffman_tree_node(root->character,
                                           1,
                                           true);
        bit_string b;
        b.append_bit(false);
        map[root->character] = std::move(b);
        return map;
    }
    
    bit_string code_word;
    infer_encoder_map_impl(code_word, root, map);
    return map;
}

int8_t huffman_tree::decode_bit_string(size_t& index, bit_string& bits)
{
    if (root->is_leaf)
    {
        index++;
        return root->character;
    }
    
    huffman_tree_node* current_node = root;
    
    while (!current_node->is_leaf)
    {
        bool bit = bits.read_bit(index++);
        current_node = (bit ? current_node->right : current_node->left);
    }
    
    return current_node->character;
}

void huffman_tree::infer_encoder_map_impl(
                            bit_string& current_code_word,
                            huffman_tree::huffman_tree_node* node,
                            std::map<int8_t, bit_string>& map)
{
    if (node->is_leaf)
    {
        map[node->character] = bit_string(current_code_word);
        return;
    }
    
    current_code_word.append_bit(false);
    infer_encoder_map_impl(current_code_word,
                           node->left,
                           map);
    current_code_word.remove_last_bit();
    
    current_code_word.append_bit(true);
    infer_encoder_map_impl(current_code_word,
                           node->right,
                           map);
    current_code_word.remove_last_bit();
}

huffman_tree::huffman_tree_node* huffman_tree::merge(huffman_tree_node* node1,
                                                     huffman_tree_node* node2)
{
    huffman_tree_node* new_node = new huffman_tree_node(0,
                                                        node1->weight +
                                                        node2->weight,
                                                        false);
    if (node1->weight < node2->weight)
    {
        new_node->left  = node1;
        new_node->right = node2;
    }
    else
    {
        new_node->left  = node2;
        new_node->right = node1;
    }
    
    return new_node;
}

uint32_t huffman_tree::check_count(uint32_t count)
{
    if (count == 0)
    {
        throw std::runtime_error{"The input count is zero."};
    }
    
    return count;
}
