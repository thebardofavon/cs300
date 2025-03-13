#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bptree.h"
#include "key.h"

// TODO: Handle root underflow in delete_recursive
// TODO: Implement recursive freeing of nodes

struct BPlusTreeNode {
    int is_leaf;              // 1 if leaf node, 0 if internal node
    int num_keys;             
    Key keys[5];             // order = 5 for now
    struct BPlusTreeNode* children[6];  
    struct BPlusTreeNode* next_leaf;   
};

// Create a new B+ tree node
BPlusTreeNode* create_node(int is_leaf, int order) {
    BPlusTreeNode* node = (BPlusTreeNode*)malloc(sizeof(BPlusTreeNode));
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    memset(node->keys, 0, order * sizeof(Key));
    for (int i = 0; i <= order; i++) {
        node->children[i] = NULL;
    }
    node->next_leaf = NULL;
    return node;
}

// Create a new B+ tree
BPlusTree* create_bptree(int order) {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = create_node(1, order);  // Root is a leaf node
    tree->order = order;
    return tree;
}

// Split a node
BPlusTreeNode* split_node(BPlusTreeNode* node, int order, Key* new_key, BPlusTreeNode** new_node) {
    int mid = (order + 1) / 2;
    *new_node = create_node(node->is_leaf, order);

    if (node->is_leaf) {
        memcpy((*new_node)->keys, &node->keys[mid], (order - mid) * sizeof(Key));
        (*new_node)->num_keys = order - mid;
        node->num_keys = mid;
        (*new_node)->next_leaf = node->next_leaf;
        node->next_leaf = *new_node;
    } else {
        memcpy((*new_node)->keys, &node->keys[mid], (order - mid) * sizeof(Key));
        memcpy((*new_node)->children, &node->children[mid], (order - mid + 1) * sizeof(BPlusTreeNode*));
        (*new_node)->num_keys = order - mid;
        node->num_keys = mid;
    }

    *new_key = node->keys[mid - 1];
    return *new_node;
}

// Insert into a non-full node
void insert_non_full(BPlusTreeNode* node, Key key, Pointer ptr, int order) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        while (i >= 0 && compare_keys(key, node->keys[i]) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->num_keys++;
    } else {
        while (i >= 0 && compare_keys(key, node->keys[i]) < 0) {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == order - 1) {
            Key new_key;
            BPlusTreeNode* new_node = NULL;
            split_node(node->children[i], order, &new_key, &new_node);
            node->keys[i] = new_key;
            node->children[i + 1] = new_node;
            node->num_keys++;

            if (compare_keys(key, new_key) > 0) {
                i++;
            }
        }
        insert_non_full(node->children[i], key, ptr, order);
    }
}

// Insert a key-pointer pair
void insert(BPlusTree* tree, Key key, Pointer ptr) {
    Key new_key;
    BPlusTreeNode* new_node = NULL;

    if (tree->root->num_keys == tree->order - 1) {
        BPlusTreeNode* new_root = create_node(0, tree->order);
        new_root->children[0] = tree->root;
        split_node(tree->root, tree->order, &new_key, &new_node);
        new_root->keys[0] = new_key;
        new_root->num_keys = 1;
        new_root->children[1] = new_node;
        tree->root = new_root;
    }

    insert_non_full(tree->root, key, ptr, tree->order);
}

// Search for a key
Pointer search_recursive(BPlusTreeNode* node, Key key) {
    int i = 0;
    while (i < node->num_keys && compare_keys(key, node->keys[i]) > 0) {
        i++;
    }

    if (node->is_leaf) {
        if (i < node->num_keys && compare_keys(key, node->keys[i]) == 0) {
            return 0;  // Simulated pointer
        }
        return -1;  // Key not found
    } else {
        return search_recursive(node->children[i], key);
    }
}

Pointer get_row(BPlusTree* tree, Key key) {
    return search_recursive(tree->root, key);
}

// Merge nodes (for deletion)
void merge_nodes(BPlusTreeNode* child1, BPlusTreeNode* child2, BPlusTreeNode* parent, int index, int order) {
    child1->keys[child1->num_keys] = parent->keys[index];
    child1->num_keys++;

    memcpy(&child1->keys[child1->num_keys], child2->keys, child2->num_keys * sizeof(Key));
    child1->num_keys += child2->num_keys;
    if (!child1->is_leaf) {
        memcpy(&child1->children[child1->num_keys], child2->children, (child2->num_keys + 1) * sizeof(BPlusTreeNode*));
    }
    child1->next_leaf = child2->next_leaf;

    for (int i = index; i < parent->num_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->num_keys--;
    free(child2);
}

// Redistribute keys
void redistribute_nodes(BPlusTreeNode* child1, BPlusTreeNode* child2, BPlusTreeNode* parent, int index, int order) {
    if (child1->num_keys < child2->num_keys) {
        for (int i = child1->num_keys; i > 0; i--) {
            child1->keys[i] = child1->keys[i - 1];
        }
        child1->keys[0] = parent->keys[index];
        parent->keys[index] = child2->keys[0];
        child1->num_keys++;

        for (int i = 0; i < child2->num_keys - 1; i++) {
            child2->keys[i] = child2->keys[i + 1];
        }
        child2->num_keys--;
    } else {
        for (int i = child2->num_keys; i > 0; i--) {
            child2->keys[i] = child2->keys[i - 1];
        }
        child2->keys[0] = parent->keys[index];
        parent->keys[index] = child1->keys[child1->num_keys - 1];
        child2->num_keys++;
        child1->num_keys--;
    }
}

// Delete a key
void delete_recursive(BPlusTreeNode* node, Key key, int order) {
    int i = 0;
    while (i < node->num_keys && compare_keys(key, node->keys[i]) > 0) {
        i++;
    }

    if (node->is_leaf) {
        if (i < node->num_keys && compare_keys(key, node->keys[i]) == 0) {
            for (int j = i; j < node->num_keys - 1; j++) {
                node->keys[j] = node->keys[j + 1];
            }
            node->num_keys--;
        }
    } else {
        if (i < node->num_keys && compare_keys(key, node->keys[i]) == 0) {
            i++;
        }
        delete_recursive(node->children[i], key, order);

        if (node->children[i]->num_keys < (order + 1) / 2) {
            if (i > 0 && node->children[i - 1]->num_keys > (order + 1) / 2) {
                redistribute_nodes(node->children[i - 1], node->children[i], node, i - 1, order);
            } else if (i < node->num_keys && node->children[i + 1]->num_keys > (order + 1) / 2) {
                redistribute_nodes(node->children[i], node->children[i + 1], node, i, order);
            } else if (i > 0) {
                merge_nodes(node->children[i - 1], node->children[i], node, i - 1, order);
            } else {
                merge_nodes(node->children[i], node->children[i + 1], node, i, order);
            }
        }
    }
}

void del_row(BPlusTree* tree, Key key) {
    delete_recursive(tree->root, key, tree->order);
    // TODO: Handle root underflow 
}

// Get next row
Pointer get_next_row(BPlusTree* tree, Key key, Key last) {
    BPlusTreeNode* leaf = tree->root;
    
    while (!leaf->is_leaf) {
        int i = 0;
        while (i < leaf->num_keys && compare_keys(last, leaf->keys[i]) > 0) {
            i++;
        }
        leaf = leaf->children[i];
    }

    int i = 0;
    while (i < leaf->num_keys && compare_keys(last, leaf->keys[i]) < 0) {
        i++;
    }

    if (i < leaf->num_keys) {
        return 0;  // Simulated pointer for the next key
    } else if (leaf->next_leaf != NULL) {
        leaf = leaf->next_leaf;
        if (leaf->num_keys > 0) {
            return 0;  // Simulated pointer for first key in next leaf
        }
    }
    return -1;  // No next row found
}

// Free B+ tree 
void free_bptree(BPlusTree* tree) {
    // TODO: Implement recursive freeing of nodes
    free(tree->root);
    free(tree);
}