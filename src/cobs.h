#ifndef COBS_H
#define COBS_H

#include <stddef.h>
#include <stdint.h>

/** Worst-case encoded size of `len` bytes, not counting the 0x00 frame delimiter. */
#define COBS_MAX_ENCODED(len) ((len) + (len) / 254u + 1u)

/** Encodes `len` bytes into `out`, which must hold COBS_MAX_ENCODED(len). Returns the length. */
size_t cobs_encode(const uint8_t *data, size_t len, uint8_t *out);

#endif
