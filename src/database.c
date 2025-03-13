#include <stddef.h>
#include "database.h"
#include "bptree.h"
#include "hashtable.h"

static HashTable* ht = NULL;

void init_database(int hash_table_size) {
    ht = create_hashtable(hash_table_size);
}

Pointer db_get_row(int table_id, Key key) {
    BPlusTree* tree = hashtable_get(ht, table_id);
    if (tree) {
        return get_row(tree, key);
    }
    return -1;  // Table not found
}

void db_insert(int table_id, Key key, Pointer ptr) {
    BPlusTree* tree = hashtable_get(ht, table_id);
    if (!tree) {
        tree = create_bptree(5);  // Order 5 for simplicity
        hashtable_insert(ht, table_id, tree);
    }
    insert(tree, key, ptr);
}

void db_delete(int table_id, Key key) {
    BPlusTree* tree = hashtable_get(ht, table_id);
    if (tree) {
        del_row(tree, key);
    }
}

Pointer db_get_next_row(int table_id, Key key, Key last) {
    BPlusTree* tree = hashtable_get(ht, table_id);
    if (tree) {
        return get_next_row(tree, key, last);
    }
    return -1;  // Table not found
}

void shutdown_database() {
    if (ht) {
        free_hashtable(ht);
        ht = NULL;
    }
}