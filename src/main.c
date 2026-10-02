#include "board.h"
#include "shell.h"
#include "uart.h"

int main(void)
{
    board_init();
    uart_init(UART_CONSOLE, 115200u);
    uart_init(UART_TELEMETRY, 115200u);
    shell_init();

    /* Never returns: ST's startup code has nothing to return to. */
    for (;;) {
        uint8_t byte;

        if (uart_read(UART_CONSOLE, &byte, 1u) == 1u) {
            shell_input(byte);
        }
    }
}
