#ifndef KEY_H
#define KEY_H

typedef struct {
    int table_id;  
    int row_id;
} Key;

int compare_keys(Key k1, Key k2);

#endif