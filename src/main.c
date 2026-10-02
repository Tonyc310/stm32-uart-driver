#include "board.h"
#include "uart.h"

int main(void)
{
    board_init();
    uart_init(UART_CONSOLE, 115200u);

    for (;;) {
        uint8_t chunk[32];
        size_t received = uart_read(UART_CONSOLE, chunk, sizeof chunk);

        for (size_t sent = 0; sent < received;) {
            sent += uart_write(UART_CONSOLE, &chunk[sent], received - sent);
        }
    }
}
