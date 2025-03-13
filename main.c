#include <stdio.h>
#include "database.h"

int main() {
    init_database(10);  

    Key key1 = {1, 100};
    Pointer ptr1 = 0;

    db_insert(1, key1, ptr1);
    printf("Inserted key (table_id: %d, row_id: %d)\n", key1.table_id, key1.row_id);

    Pointer result = db_get_row(1, key1);
    if (result != -1) {
        printf("Found key\n");
    }

    db_delete(1, key1);
    printf("Deleted key\n");

    shutdown_database();
    return 0;
}