#include "shell.h"

#include "board.h"
#include "uart.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define MAX_LINE 64u
#define PROMPT "> "

typedef struct {
    const char *name;
    const char *usage;
    void (*run)(const char *args);
} command_t;

static char line[MAX_LINE + 1u];
static size_t line_len;
static bool last_was_cr;

static void write_all(const char *text, size_t len)
{
    while (len > 0u) {
        size_t sent = uart_write(UART_CONSOLE, (const uint8_t *)text, len);
        text += sent;
        len -= sent;
    }
}

static void print(const char *text)
{
    write_all(text, strlen(text));
}

static void print_u32(uint32_t value)
{
    char digits[10];
    size_t count = 0;

    do {
        digits[sizeof digits - 1u - count] = (char)('0' + value % 10u);
        value /= 10u;
        count++;
    } while (value > 0u);
    write_all(&digits[sizeof digits - count], count);
}

static void cmd_help(const char *args);

static void cmd_led(const char *args)
{
    if (strcmp(args, "on") == 0) {
        board_led_set(true);
    } else if (strcmp(args, "off") == 0) {
        board_led_set(false);
    } else {
        print("usage: led on|off\r\n");
    }
}

static void cmd_uptime(const char *args)
{
    (void)args;
    print_u32(board_uptime_ms());
    print(" ms\r\n");
}

static void cmd_echo(const char *args)
{
    print(args);
    print("\r\n");
}

static const command_t commands[] = {
    {"help", "help          list commands", cmd_help},
    {"led", "led on|off    switch the green LED", cmd_led},
    {"uptime", "uptime        milliseconds since reset", cmd_uptime},
    {"echo", "echo <text>   print text back", cmd_echo},
};

static void cmd_help(const char *args)
{
    (void)args;
    for (size_t i = 0; i < sizeof commands / sizeof commands[0]; i++) {
        print(commands[i].usage);
        print("\r\n");
    }
}

static void run_line(void)
{
    while (line_len > 0u && line[line_len - 1u] == ' ') {
        line_len--;
    }
    line[line_len] = '\0';

    char *name = line;
    while (*name == ' ') {
        name++;
    }
    if (*name == '\0') {
        return;
    }

    char *args = strchr(name, ' ');
    if (args == NULL) {
        args = &line[line_len];
    } else {
        *args++ = '\0';
        while (*args == ' ') {
            args++;
        }
    }

    for (size_t i = 0; i < sizeof commands / sizeof commands[0]; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            commands[i].run(args);
            return;
        }
    }
    print("unknown command: ");
    print(name);
    print(" (try help)\r\n");
}

void shell_init(void)
{
    line_len = 0u;
    last_was_cr = false;
    print("\r\nstm32-uart-driver, type help\r\n" PROMPT);
}

void shell_input(uint8_t byte)
{
    bool after_cr = last_was_cr;
    last_was_cr = (byte == '\r');

    if (byte == '\r' || byte == '\n') {
        /* Terminals end lines with CR, LF, or CR LF; the LF of a CR LF pair is not a second line.
         */
        if (byte == '\n' && after_cr) {
            return;
        }
        print("\r\n");
        run_line();
        line_len = 0u;
        print(PROMPT);
    } else if (byte == '\b' || byte == 0x7Fu) {
        /* Backspace arrives as BS or DEL depending on the terminal. */
        if (line_len > 0u) {
            line_len--;
            print("\b \b");
        }
    } else if (byte >= ' ' && byte <= '~' && line_len < MAX_LINE) {
        line[line_len++] = (char)byte;
        write_all((const char *)&byte, 1u);
    }
}
