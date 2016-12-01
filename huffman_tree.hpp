#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include "bit_string.hpp"
#include <cstdint>
#include <map>

class huffman_tree
{
public:
    /******************************************************
    * Build this Huffman tree using the character counts. *
    ******************************************************/
    explicit huffman_tree(std::map<int8_t, uint32_t>& count_map);
    
    ~huffman_tree();
    
    /*****************************************
    * Infers the encoder map from this tree. *
    *****************************************/ 
    std::map<int8_t, bit_string> infer_encoder_map();
    
    /***************************************************************************
    * Decodes the next character from the bit string starting at bit with      *
    * index 'start_index'. This method will advance the value of 'start_index' *
    * by the code word length read from the tree.                              *
    ***************************************************************************/
    int8_t decode_bit_string(size_t& start_index, bit_string& bits);
    
private:
    
    // The actual Huffman tree node type:
    struct huffman_tree_node
    {
        int8_t             character; // The character of this node. Ignore if
                                      // not a leaf node.
        float              weight;    // If a leaf, the weight of the character.
                                      // Otherwise, the sum of weights of its
                                      // left and right child nodes.
        bool               is_leaf;   // This node is leaf?
        huffman_tree_node* left;      // The left child node.
        huffman_tree_node* right;     // The right child node.
        
        // Construct a new Huffman tree node.
        huffman_tree_node(int8_t character,
                          float weight,
                          bool is_leaf)
        :
        character   {character},
        weight      {weight},
        is_leaf     {is_leaf},
        left        {nullptr},
        right       {nullptr}
        {}
    };
    
    // The root node of this Huffman tree:
    huffman_tree_node* root;
    
    // Merges the two input into a new node that has 'node1' and 'node2' as its
    // children. The node with lower ...
    huffman_tree_node* merge(huffman_tree_node* node1,
                             huffman_tree_node* node2);
    
    // The recursive implementation of the routine that builds the encoder map:
    void infer_encoder_map_impl(bit_string& bit_string_builder,
                                huffman_tree_node* current_node,
                                std::map<int8_t, bit_string>& map);
    
    // Checks that the input count is positive:
    uint32_t check_count(uint32_t count);
    
    // Used for deallocating the memory occupied by the tree nodes:
    void recursive_node_delete(huffman_tree_node* node);
    
public:
    
    // Used for the priority queue:
    class huffman_tree_node_comparator {
    public:
        bool operator()(const huffman_tree_node *const lhs,
                        const huffman_tree_node *const rhs)
        {
            if (lhs->weight == rhs->weight)
            {
                // If same weights, order by char value:
                return lhs->character > rhs->character;
            }
            
            // Otherwise, compare by weights:
            return lhs->weight > rhs->weight;
        }
    };
};


#endif // HUFFMAN_TREE_HPP
