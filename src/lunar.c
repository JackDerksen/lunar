#include "lunar.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

/* Architecture sketch only, deliberately no function bodies. */

struct lunar_entry {
  unsigned char *key; /* Owned; NULL marks an unused slot. */
  uint32_t key_size;
  off_t record_offset; /* Start of the record's length fields. */
};

struct lunar_db {
  int descriptor;
  off_t append_offset;
  struct lunar_entry *entries;
  size_t capacity;
  size_t count;
  bool failed; /* An uncertain append permits only close. */
};

/*
 * Index
 *   Start with 16 slots. Double before exceeding half occupancy.
 *   Hash bytes with 64-bit FNV-1a and probe linearly, comparing full keys.
 *   Rehash into a fresh array; transfer key ownership after allocation
 * succeeds. Check allocation arithmetic before growth. ponytail: non-keyed
 * hashing assumes trusted keys; use a keyed hash if hostile inputs become a
 * requirement.
 *
 * lunar_open
 *   Validate inputs, clear the output, open read/write without truncation.
 *   Write the eight-byte magic/version header only if the file is empty.
 *   Otherwise validate the entire header, then replay from byte eight to EOF:
 *     Decode lengths from little-endian bytes.
 *     Reject empty keys, incomplete records, and arithmetic overflow.
 *     Check the complete record fits before allocating or reading its key.
 *     Upsert key -> record start offset; skip value bytes.
 *   Set append_offset to EOF and return the fully constructed handle.
 *   On failure, free partial state and close. Never repair/truncate the file.
 *
 * lunar_put
 *   Validate inputs, failed state, lengths, and resulting file offset.
 *   Find the key. For a new key, reserve capacity and copy its bytes first.
 *   Re-find the slot after growth; rehashing invalidates slot pointers.
 *   Encode lengths in an eight-byte stack buffer.
 *   Append header, key, and value with complete pwrite loops.
 *   On I/O failure, mark failed and free any unpublished key copy.
 *   After all bytes succeed, publish the slot and append offset without any
 *   further fallible work. Replacement retains the existing owned key.
 *
 * lunar_get
 *   Clear outputs, validate arguments, reject failed handles, probe the key.
 *   Return NOT_FOUND if absent.
 *   Read and validate record lengths at the stored offset.
 *   Check the value position and extent, allocate its copy, and read fully.
 *   Publish outputs after success; empty values allocate nothing.
 *
 * lunar_close
 *   NULL succeeds. Otherwise close the descriptor and release keys, table,
 *   and handle, even if close reports an error.
 *
 * Private helpers own probing, record-header encoding/decoding, and complete
 * positional reads/writes. Retry EINTR, advance on short I/O, and chunk
 * requests to fit ssize_t. Unexpected read EOF is CORRUPT; write zero-progress
 * and other I/O failures are IO_ERROR. Never serialise the structs above.
 */
