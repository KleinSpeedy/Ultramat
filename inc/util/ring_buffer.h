#ifndef RING_BUFFER_H
#define RING_BUFFER_H

/**
 * This is basically a queue but only with integers as to avoid mixing with
 * command queues
 */

#include <stdint.h>

#define RING_BUFFER_MAX_SIZE    32
typedef struct ring_buffer
{
    int head, tail;

    uint8_t buffer[RING_BUFFER_MAX_SIZE];
} ring_buffer_t;

// put data into buffer and advance head
int ring_buffer_put(ring_buffer_t * const rb, uint8_t data);

// store current item in data and advance tail
int ring_buffer_pop(ring_buffer_t * const rb, uint8_t *data);

// like pop but we dont advance tail
int ring_buffer_peek(ring_buffer_t * const rb, uint8_t *data);

int ring_buffer_is_empty(ring_buffer_t * const rb);

int ring_buffer_is_full(ring_buffer_t * const rb);

// check if ring buffer contains item and return its index
// else returns -1
int ring_buffer_find(ring_buffer_t * const rb, uint8_t item);

#endif // RING_BUFFER_H
