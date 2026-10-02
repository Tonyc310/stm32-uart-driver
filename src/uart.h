#ifndef UART_H
#define UART_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    UART_CONSOLE, /* USART2 */
    UART_COUNT,
} uart_id_t;

/** Starts the port at `baud`, 8N1, with interrupt-driven RX and TX. Call after board_init(). */
void uart_init(uart_id_t id, uint32_t baud);

/** Queues up to `len` bytes to send and returns how many fit. Never blocks. */
size_t uart_write(uart_id_t id, const uint8_t *data, size_t len);

/** Copies up to `len` received bytes into `data` and returns how many. Never blocks. */
size_t uart_read(uart_id_t id, uint8_t *data, size_t len);

/** Received bytes lost to a full RX buffer or a hardware overrun since uart_init(). */
uint32_t uart_rx_dropped(uart_id_t id);

#endif
