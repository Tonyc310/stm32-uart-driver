#include "telemetry.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

static void expect_frame(const uint8_t *expected, size_t expected_len, const uint8_t *frame,
                         size_t len)
{
    TEST_ASSERT_EQUAL(expected_len, len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, frame, len);
}

/* Whole frames cover the CRC, COBS (both packets contain zero bytes), and each message's layout. */
static void test_frames_match_reference_bytes(void)
{
    /* Generated with the Python `cobs` and `crcmod` packages, independent of this code. */
    const uint8_t status[] = {0x04, 0x01, 0xE8, 0x03, 0x01, 0x04, 0x01, 0x6C, 0xF9, 0x00};
    const uint8_t console_stats[] = {0x03, 0x02, 0x0D, 0x01, 0x01, 0x02, 0x64, 0x01, 0x01,
                                     0x01, 0x01, 0x01, 0x01, 0x03, 0x16, 0x35, 0x00};
    const uart_stats_t stats = {.rx_bytes = 13, .tx_bytes = 100, .rx_dropped = 0};
    uint8_t frame[TELEMETRY_MAX_FRAME];
    size_t len;

    len = telemetry_pack_status(1000, true, frame);
    expect_frame(status, sizeof status, frame, len);
    len = telemetry_pack_console_stats(&stats, frame);
    expect_frame(console_stats, sizeof console_stats, frame, len);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_frames_match_reference_bytes);
    return UNITY_END();
}
