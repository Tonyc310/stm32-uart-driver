#include "cobs.h"

/*
 * Each block starts with a code byte: the distance to the next zero, which is left out of the
 * output. A code of 0xFF means 254 data bytes with no zero after them.
 */
size_t cobs_encode(const uint8_t *data, size_t len, uint8_t *out)
{
    size_t code_at = 0;
    size_t written = 1;
    uint8_t code = 1;

    for (size_t i = 0; i < len; i++) {
        if (data[i] != 0u) {
            out[written++] = data[i];
            code++;
        }
        if (data[i] == 0u || code == 0xFFu) {
            out[code_at] = code;
            code_at = written++;
            code = 1;
        }
    }
    out[code_at] = code;
    return written;
}
