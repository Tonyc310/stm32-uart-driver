#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

/** Lock-free byte queue for one producer and one consumer, e.g. an ISR and the main loop. */
typedef struct {
    uint8_t *data;
    uint32_t mask;
    _Atomic uint32_t head; /* written only by the producer */
    _Atomic uint32_t tail; /* written only by the consumer */
} ring_buffer_t;

/** Sets up `rb` over `storage`. Returns false unless `size` is a nonzero power of two. */
bool rb_init(ring_buffer_t *rb, uint8_t *storage, uint32_t size);

/** Producer only. Returns false if the buffer is full. */
bool rb_push(ring_buffer_t *rb, uint8_t byte);

/** Consumer only. Returns false if the buffer is empty. */
bool rb_pop(ring_buffer_t *rb, uint8_t *byte);

/** Bytes queued. A snapshot: the other side may change it right after. */
uint32_t rb_count(const ring_buffer_t *rb);

#endif
