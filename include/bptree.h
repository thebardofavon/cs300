#ifndef BPTREE_H
#define BPTREE_H

#include "key.h"

typedef int Pointer;

typedef struct BPlusTreeNode BPlusTreeNode;

typedef struct {
    BPlusTreeNode* root;
    int order;  
} BPlusTree;

BPlusTree* create_bptree(int order);
void insert(BPlusTree* tree, Key key, Pointer ptr);
Pointer get_row(BPlusTree* tree, Key key);
void del_row(BPlusTree* tree, Key key);
Pointer get_next_row(BPlusTree* tree, Key key, Key last);
void free_bptree(BPlusTree* tree);

#endif