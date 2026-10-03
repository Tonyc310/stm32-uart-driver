#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

/** Enables clocks, routes the console UART to PA2/PA3 and telemetry to PD8/PD9, starts the tick. */
void board_init(void);

/** Switches the green LED (PD12). */
void board_led_set(bool on);

/** True while the green LED is on. */
bool board_led_is_on(void);

/** Milliseconds since board_init(); wraps after about 49 days. */
uint32_t board_uptime_ms(void);

#endif
