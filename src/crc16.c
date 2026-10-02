#include "crc16.h"

#define POLY 0x1021u

/* Bitwise rather than table-driven: telemetry is a few bytes a second, so flash matters more. */
uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFu;

    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)(data[i] << 8);
        for (int bit = 0; bit < 8; bit++) {
            crc = (crc & 0x8000u) ? (uint16_t)((crc << 1) ^ POLY) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}
