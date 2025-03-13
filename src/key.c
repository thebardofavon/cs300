#include "key.h"

int compare_keys(Key k1, Key k2) {
    if (k1.table_id != k2.table_id) {
        return k1.table_id - k2.table_id;
    }
    return k1.row_id - k2.row_id;
}