#include "ring_buffer.h"
#include "unity.h"

#define SIZE 8u

static uint8_t storage[SIZE];
static ring_buffer_t rb;

void setUp(void)
{
    TEST_ASSERT_TRUE(rb_init(&rb, storage, SIZE));
}

void tearDown(void)
{
}

static void fill(uint32_t count, uint8_t first)
{
    for (uint32_t i = 0; i < count; i++) {
        TEST_ASSERT_TRUE(rb_push(&rb, (uint8_t)(first + i)));
    }
}

static void expect_drain(uint32_t count, uint8_t first)
{
    uint8_t byte;

    for (uint32_t i = 0; i < count; i++) {
        TEST_ASSERT_TRUE(rb_pop(&rb, &byte));
        TEST_ASSERT_EQUAL_UINT8((uint8_t)(first + i), byte);
    }
    TEST_ASSERT_EQUAL_UINT32(0, rb_count(&rb));
}

static void test_init_rejects_sizes_that_are_not_powers_of_two(void)
{
    TEST_ASSERT_FALSE(rb_init(&rb, storage, 0));
    TEST_ASSERT_FALSE(rb_init(&rb, storage, 6));
}

static void test_pop_fails_when_empty(void)
{
    uint8_t byte;

    TEST_ASSERT_FALSE(rb_pop(&rb, &byte));
}

static void test_push_fails_when_full_without_losing_data(void)
{
    fill(SIZE, 0);
    TEST_ASSERT_EQUAL_UINT32(SIZE, rb_count(&rb));
    TEST_ASSERT_FALSE(rb_push(&rb, 0xFF));
    expect_drain(SIZE, 0);
}

static void test_wraps_past_end_of_storage_and_counter_overflow(void)
{
    /* Start near the top of the counter range so the counters and the storage index both wrap. */
    atomic_store(&rb.head, UINT32_MAX - 2u);
    atomic_store(&rb.tail, UINT32_MAX - 2u);

    fill(SIZE, 40);
    TEST_ASSERT_FALSE(rb_push(&rb, 0xFF));
    expect_drain(SIZE, 40);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_rejects_sizes_that_are_not_powers_of_two);
    RUN_TEST(test_pop_fails_when_empty);
    RUN_TEST(test_push_fails_when_full_without_losing_data);
    RUN_TEST(test_wraps_past_end_of_storage_and_counter_overflow);
    return UNITY_END();
}
