#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include "bit_string.hpp"
#include <cstdint>
#include <map>

class huffman_tree
{
public:
    /*******************************************************
    * Build this Huffman tree using the character weights. *
    *******************************************************/
    explicit huffman_tree(std::map<uint8_t, float>& weight_map);
    
    ~huffman_tree();
    
    /*****************************************
    * Infers the encoder map from this tree. *
    *****************************************/ 
    std::map<uint8_t, bit_string> infer_encoder_map();
    
private:
    
    struct huffman_tree_node
    {
        uint8_t            character;
        float              weight;
        bool               is_leaf;
        huffman_tree_node* left;
        huffman_tree_node* right;
        
        huffman_tree_node(uint8_t character,
                          float weight,
                          bool is_leaf)
        :
        character   {character},
        weight      {weight},
        is_leaf     {is_leaf},
        left        {nullptr},
        right       {nullptr}
        {}
        
        static bool cmp(huffman_tree_node* node1,
                        huffman_tree_node* node2)
        {
            return node1->weight < node2->weight;
        }
    };
    
    huffman_tree_node* root;
    
    huffman_tree_node* merge(huffman_tree_node* node1,
                             huffman_tree_node* node2);
    
    void infer_encoder_map_impl(bit_string& bit_string_builder,
                                huffman_tree_node* current_node,
                                std::map<uint8_t, bit_string>& map);
    
    float check_weight(float weight);
    
    void recursive_node_delete(huffman_tree_node* node);
    
public:
    
    class huffman_tree_node_comparator {
    public:
        bool operator()(const huffman_tree_node *const lhs,
                        const huffman_tree_node *const rhs)
        {
            return lhs->weight > rhs->weight;
        }
    };
    
};


#endif // HUFFMAN_TREE_HPP
