#include "lunar.h"

#include <stdio.h>
#include <stdlib.h>

/* Proposed usage; syntax-checked, but not yet linkable. */
int main(int argument_count, char **arguments) {
    if (argument_count != 2) {
        fprintf(stderr, "usage: %s DATABASE_PATH\n", arguments[0]);
        return EXIT_FAILURE;
    }

    lunar_db *database = NULL;
    lunar_status status = lunar_open(arguments[1], &database);
    if (status != LUNAR_OK) {
        fprintf(stderr, "open failed: %d\n", (int)status);
        return EXIT_FAILURE;
    }

    status = lunar_put(database, "name", 4, "Jack", 4);
    if (status == LUNAR_OK) {
        status = lunar_put(database, "name", 4, "Jill", 4);
    }

    void *value = NULL;
    size_t value_size = 0;
    if (status == LUNAR_OK) {
        status = lunar_get(database, "name", 4, &value, &value_size);
    }
    if (status == LUNAR_OK && value_size > 0) {
        fwrite(value, 1, value_size, stdout);
        putchar('\n');
    }
    free(value);

    lunar_status close_status = lunar_close(database);
    if (status != LUNAR_OK || close_status != LUNAR_OK) {
        fprintf(stderr, "database operation failed: %d; close: %d\n",
                (int)status, (int)close_status);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
