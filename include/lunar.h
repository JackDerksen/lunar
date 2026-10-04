#ifndef LUNAR_H
#define LUNAR_H

#include <stddef.h>

typedef struct lunar_db lunar_db;

typedef enum {
  LUNAR_OK = 0,
  LUNAR_NOT_FOUND,
  LUNAR_INVALID_ARGUMENT,
  LUNAR_OUT_OF_MEMORY,
  LUNAR_IO_ERROR,
  LUNAR_CORRUPT,
  LUNAR_LIMIT_EXCEEDED
} lunar_status;

/*
 * Proposed interface; function bodies are not implemented yet.
 * One owner per file, one call at a time. No crash durability guarantee.
 * Inputs are borrowed during calls. Keys are nonempty binary strings;
 * values may be empty. Lengths must fit uint32_t and file positions off_t.
 * Invalid pointers/empty keys return INVALID_ARGUMENT; lengths or positions
 * exceeding these limits return LIMIT_EXCEEDED.
 */

/*
 * Open/create path, initialise an empty file, and rebuild the index.
 * Malformed, incomplete, or unsupported files return CORRUPT without repair.
 * A valid output pointer is cleared before work. Failure releases all acquired
 * resources; failed initialisation can leave a partial file header on disk.
 */
lunar_status lunar_open(const char *path, lunar_db **database);

/*
 * Append a replacement, then publish its offset without further allocation.
 * value may be NULL only when value_size is zero.
 * Allocation and limit failures happen before writing any record bytes.
 * Append I/O failure leaves the index unchanged and disables put/get until
 * close. Persistence is uncertain; failure does not imply rollback.
 */
lunar_status lunar_put(lunar_db *database, const void *key, size_t key_size,
                     const void *value, size_t value_size);

/*
 * Return a caller-owned copy; release it with free(). Both outputs are
 * required. Valid outputs are cleared before work and remain cleared on
 * failure. Empty values return OK with NULL and zero; missing keys return
 * NOT_FOUND.
 */
lunar_status lunar_get(lunar_db *database, const void *key, size_t key_size,
                     void **value, size_t *value_size);

/*
 * Release the handle even on IO_ERROR; never retry using the consumed handle.
 * NULL succeeds. Close does not synchronise the file to disk.
 */
lunar_status lunar_close(lunar_db *database);

#endif
