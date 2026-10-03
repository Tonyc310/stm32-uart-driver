#include "board.h"
#include "shell.h"
#include "telemetry.h"
#include "uart.h"

#define TELEMETRY_PERIOD_MS 1000u

/* Queues the whole frame or none of it; the next report follows within a second anyway. */
static void send_frame(const uint8_t *frame, size_t len)
{
    if (uart_tx_free(UART_TELEMETRY) >= len) {
        (void)uart_write(UART_TELEMETRY, frame, len);
    }
}

static void send_telemetry(void)
{
    uint8_t frame[TELEMETRY_MAX_FRAME];
    uart_stats_t console = uart_stats(UART_CONSOLE);
    size_t len;

    len = telemetry_pack_status(board_uptime_ms(), board_led_is_on(), frame);
    send_frame(frame, len);
    len = telemetry_pack_console_stats(&console, frame);
    send_frame(frame, len);
}

int main(void)
{
    board_init();
    uart_init(UART_CONSOLE, 115200u);
    uart_init(UART_TELEMETRY, 115200u);
    shell_init();

    uint32_t last_report_ms = board_uptime_ms();

    /* Never returns: ST's startup code has nothing to return to. */
    for (;;) {
        uint8_t byte;

        if (uart_read(UART_CONSOLE, &byte, 1u) == 1u) {
            shell_input(byte);
        }
        /* Unsigned subtraction stays correct when the millisecond counter wraps. */
        if (board_uptime_ms() - last_report_ms >= TELEMETRY_PERIOD_MS) {
            last_report_ms += TELEMETRY_PERIOD_MS;
            send_telemetry();
        }
    }
}
