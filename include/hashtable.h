#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "bptree.h"

typedef struct {
    struct HashEntry** buckets;
    int size;
} HashTable;

HashTable* create_hashtable(int size);
void hashtable_insert(HashTable* ht, int table_id, BPlusTree* tree);
BPlusTree* hashtable_get(HashTable* ht, int table_id);
void free_hashtable(HashTable* ht);

#endif