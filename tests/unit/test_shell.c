#include "board.h"
#include "shell.h"
#include "uart.h"
#include "unity.h"

#include <string.h>

static char output[1024];
static size_t output_len;
static bool led_on;
static uint32_t uptime_ms;

/* Link-time fakes for the UART and board drivers, so the shell runs on the host. */
size_t uart_write(uart_id_t id, const uint8_t *data, size_t len)
{
    TEST_ASSERT_EQUAL(UART_CONSOLE, id);
    TEST_ASSERT_LESS_THAN(sizeof output, output_len + len);
    memcpy(&output[output_len], data, len);
    output_len += len;
    output[output_len] = '\0';
    return len;
}

uart_stats_t uart_stats(uart_id_t id)
{
    (void)id;
    return (uart_stats_t){0};
}

void board_led_set(bool on)
{
    led_on = on;
}

uint32_t board_uptime_ms(void)
{
    return uptime_ms;
}

static void type(const char *text)
{
    while (*text != '\0') {
        shell_input((uint8_t)*text++);
    }
}

static void expect_output(const char *text)
{
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(output, text), output);
}

void setUp(void)
{
    led_on = false;
    uptime_ms = 0;
    shell_init();
    output_len = 0;
    output[0] = '\0';
}

void tearDown(void)
{
}

static void test_line_editing_handles_backspace_and_crlf(void)
{
    type("echoo\b hi\r\n");
    expect_output("\r\nhi\r\n> ");
    TEST_ASSERT_NULL(strstr(output, "> \r\n> "));
}

static void test_ignores_input_past_line_limit(void)
{
    for (int i = 0; i < 200; i++) {
        type("x");
    }
    type("\r");
    expect_output("unknown command: xxxx");
}

static void test_led_and_uptime_commands_use_the_board(void)
{
    type("led on\r");
    TEST_ASSERT_TRUE(led_on);
    type("led off\r");
    TEST_ASSERT_FALSE(led_on);

    uptime_ms = 4294967295u;
    type("uptime\r");
    expect_output("\r\n4294967295 ms\r\n");
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_line_editing_handles_backspace_and_crlf);
    RUN_TEST(test_ignores_input_past_line_limit);
    RUN_TEST(test_led_and_uptime_commands_use_the_board);
    return UNITY_END();
}
