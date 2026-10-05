#ifndef UART_PORTS_H
#define UART_PORTS_H

/* This board's UART ports; board.c binds each one to a USART and its pins. */
typedef enum {
    UART_CONSOLE,
    UART_TELEMETRY,
    UART_COUNT,
} uart_id_t;

#endif
