#ifndef DATABASE_H
#define DATABASE_H

#include "bptree.h"
#include "hashtable.h"

void init_database(int hash_table_size);
Pointer db_get_row(int table_id, Key key);
void db_insert(int table_id, Key key, Pointer ptr);
void db_delete(int table_id, Key key);
Pointer db_get_next_row(int table_id, Key key, Key last);
void shutdown_database();

#endif