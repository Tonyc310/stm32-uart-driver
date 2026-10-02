#include "cobs.h"
#include "crc16.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

static void expect_cobs(const uint8_t *data, size_t len, const uint8_t *expected,
                        size_t expected_len)
{
    uint8_t out[16];

    TEST_ASSERT_EQUAL(expected_len, cobs_encode(data, len, out));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, out, expected_len);
}

static void test_crc_matches_standard_check_value(void)
{
    const uint8_t check[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

    TEST_ASSERT_EQUAL_HEX16(0x29B1, crc16_ccitt(check, sizeof check));
}

static void test_cobs_matches_reference_encodings(void)
{
    const uint8_t mixed[] = {0x11, 0x22, 0x00, 0x33};
    const uint8_t mixed_encoded[] = {0x03, 0x11, 0x22, 0x02, 0x33};
    const uint8_t zeros[] = {0x00, 0x00};
    const uint8_t zeros_encoded[] = {0x01, 0x01, 0x01};

    expect_cobs(mixed, sizeof mixed, mixed_encoded, sizeof mixed_encoded);
    expect_cobs(zeros, sizeof zeros, zeros_encoded, sizeof zeros_encoded);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_crc_matches_standard_check_value);
    RUN_TEST(test_cobs_matches_reference_encodings);
    return UNITY_END();
}
