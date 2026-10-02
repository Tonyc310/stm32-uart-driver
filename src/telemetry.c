#include "telemetry.h"

#include "crc16.h"

#define ID_STATUS 0x01u
#define ID_CONSOLE_STATS 0x02u

static size_t put_u32(uint8_t *out, uint32_t value)
{
    out[0] = (uint8_t)value;
    out[1] = (uint8_t)(value >> 8);
    out[2] = (uint8_t)(value >> 16);
    out[3] = (uint8_t)(value >> 24);
    return 4u;
}

/* `packet` must have room for the two CRC bytes appended here. */
static size_t frame_packet(uint8_t *packet, size_t len, uint8_t *frame)
{
    uint16_t crc = crc16_ccitt(packet, len);

    packet[len++] = (uint8_t)crc;
    packet[len++] = (uint8_t)(crc >> 8);

    size_t framed = cobs_encode(packet, len, frame);
    frame[framed++] = 0x00u;
    return framed;
}

size_t telemetry_pack_status(uint32_t uptime_ms, bool led_on, uint8_t frame[TELEMETRY_MAX_FRAME])
{
    uint8_t packet[TELEMETRY_MAX_PACKET];
    size_t len = 0;

    packet[len++] = ID_STATUS;
    len += put_u32(&packet[len], uptime_ms);
    packet[len++] = led_on ? 1u : 0u;
    return frame_packet(packet, len, frame);
}

size_t telemetry_pack_console_stats(const uart_stats_t *stats, uint8_t frame[TELEMETRY_MAX_FRAME])
{
    uint8_t packet[TELEMETRY_MAX_PACKET];
    size_t len = 0;

    packet[len++] = ID_CONSOLE_STATS;
    len += put_u32(&packet[len], stats->rx_bytes);
    len += put_u32(&packet[len], stats->tx_bytes);
    len += put_u32(&packet[len], stats->rx_dropped);
    return frame_packet(packet, len, frame);
}
