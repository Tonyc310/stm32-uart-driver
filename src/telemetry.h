#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "cobs.h"
#include "uart.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TELEMETRY_MAX_PACKET 15u /* ID + largest payload (12) + CRC */
#define TELEMETRY_MAX_FRAME (COBS_MAX_ENCODED(TELEMETRY_MAX_PACKET) + 1u)

/*
 * A frame is one packet (ID, little-endian fields, CRC-16) COBS-encoded and ended by 0x00.
 * telemetry.toml describes the messages for serial-decoder.
 */

/** Frames a status message into `frame`; returns the frame length. */
size_t telemetry_pack_status(uint32_t uptime_ms, bool led_on, uint8_t frame[TELEMETRY_MAX_FRAME]);

/** Frames the console port's byte counters into `frame`; returns the frame length. */
size_t telemetry_pack_console_stats(const uart_stats_t *stats, uint8_t frame[TELEMETRY_MAX_FRAME]);

#endif
