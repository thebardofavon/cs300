#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "bptree.h"

typedef struct HashEntry {
    int table_id;
    BPlusTree* bptree;
    struct HashEntry* next;  
} HashEntry;

unsigned int hash_function(int table_id, int size) {
    return table_id % size;
}

// Create hash table
HashTable* create_hashtable(int size) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    ht->size = size;
    ht->buckets = (HashEntry**)calloc(size, sizeof(HashEntry*));
    return ht;
}

// Insert or update
void hashtable_insert(HashTable* ht, int table_id, BPlusTree* tree) {
    unsigned int index = hash_function(table_id, ht->size);
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    entry->table_id = table_id;
    entry->bptree = tree;
    entry->next = ht->buckets[index];
    ht->buckets[index] = entry;
}

// Get B+ tree
BPlusTree* hashtable_get(HashTable* ht, int table_id) {
    unsigned int index = hash_function(table_id, ht->size);
    HashEntry* entry = ht->buckets[index];
    while (entry != NULL) {
        if (entry->table_id == table_id) {
            return entry->bptree;
        }
        entry = entry->next;
    }
    return NULL;
}

// Free hash table
void free_hashtable(HashTable* ht) {
    for (int i = 0; i < ht->size; i++) {
        HashEntry* entry = ht->buckets[i];
        while (entry != NULL) {
            HashEntry* next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(ht->buckets);
    free(ht);
}