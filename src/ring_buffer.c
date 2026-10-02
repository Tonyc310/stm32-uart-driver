#include "ring_buffer.h"

/*
 * head and tail count bytes ever pushed and popped, wrapping at 2^32. Their difference is the
 * fill level, so all `size` slots are usable and full is never confused with empty.
 */

static uint32_t capacity(const ring_buffer_t *rb)
{
    return rb->mask + 1u;
}

bool rb_init(ring_buffer_t *rb, uint8_t *storage, uint32_t size)
{
    /* A power of two has exactly one bit set, so clearing its lowest set bit leaves zero. */
    if (size == 0u || (size & (size - 1u)) != 0u) {
        return false;
    }
    rb->data = storage;
    rb->mask = size - 1u;
    atomic_init(&rb->head, 0u);
    atomic_init(&rb->tail, 0u);
    return true;
}

bool rb_push(ring_buffer_t *rb, uint8_t byte)
{
    uint32_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);
    uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    if (head - tail == capacity(rb)) {
        return false;
    }
    rb->data[head & rb->mask] = byte;
    atomic_store_explicit(&rb->head, head + 1u, memory_order_release);
    return true;
}

bool rb_pop(ring_buffer_t *rb, uint8_t *byte)
{
    uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);
    uint32_t head = atomic_load_explicit(&rb->head, memory_order_acquire);

    if (head == tail) {
        return false;
    }
    *byte = rb->data[tail & rb->mask];
    atomic_store_explicit(&rb->tail, tail + 1u, memory_order_release);
    return true;
}

uint32_t rb_count(const ring_buffer_t *rb)
{
    /* Tail first: head never falls behind a tail read earlier, so this can't underflow. */
    uint32_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);
    uint32_t head = atomic_load_explicit(&rb->head, memory_order_acquire);

    return head - tail;
}
