#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

/** Enables peripheral clocks, routes the console UART to PA2/PA3, and starts the 1 ms tick. */
void board_init(void);

/** Switches the green LED (PD12). */
void board_led_set(bool on);

/** Milliseconds since board_init(); wraps after about 49 days. */
uint32_t board_uptime_ms(void);

#endif
