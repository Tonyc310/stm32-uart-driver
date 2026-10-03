#ifndef UART_H
#define UART_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    UART_CONSOLE,   /* USART2 */
    UART_TELEMETRY, /* USART3 */
    UART_COUNT,
} uart_id_t;

typedef struct {
    uint32_t rx_bytes;
    uint32_t tx_bytes;
    uint32_t rx_dropped; /* lost to a full RX buffer or a hardware overrun */
} uart_stats_t;

/** Starts the port at `baud`, 8N1, with interrupt-driven RX and TX. Call after board_init(). */
void uart_init(uart_id_t id, uint32_t baud);

/** Queues up to `len` bytes to send and returns how many fit. Never blocks. */
size_t uart_write(uart_id_t id, const uint8_t *data, size_t len);

/** Free space in the TX buffer. Only grows until the next uart_write(), as the ISR only drains. */
size_t uart_tx_free(uart_id_t id);

/** Copies up to `len` received bytes into `data` and returns how many. Never blocks. */
size_t uart_read(uart_id_t id, uint8_t *data, size_t len);

/** Byte counters since uart_init(). A snapshot: the ISR may update them right after. */
uart_stats_t uart_stats(uart_id_t id);

#endif
