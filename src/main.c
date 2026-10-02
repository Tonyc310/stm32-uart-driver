#include "board.h"
#include "shell.h"
#include "uart.h"

int main(void)
{
    board_init();
    uart_init(UART_CONSOLE, 115200u);
    shell_init();

    for (;;) {
        uint8_t byte;

        if (uart_read(UART_CONSOLE, &byte, 1u) == 1u) {
            shell_input(byte);
        }
    }
}
