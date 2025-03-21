#include "util/ring_buffer.h"
#include <stdio.h>

#define RING_BUFFER_CAPACITY    RING_BUFFER_MAX_SIZE - 1
#define RING_BUFFER_END(x)      x->buffer + RING_BUFFER_MAX_SIZE

static int ring_buffer_free(ring_buffer_t * const rb)
{
    if (rb->head >= rb->tail)
        return RING_BUFFER_CAPACITY - (rb->head - rb->tail);
    else
        return (rb->tail - rb->head) - 1;
}

static inline int ring_buffer_used(ring_buffer_t * const rb)
{
    return RING_BUFFER_CAPACITY - ring_buffer_free(rb);
}

int ring_buffer_put(ring_buffer_t *const rb, uint8_t data)
{
    if(!rb)
        return -1;

    if(ring_buffer_is_full(rb))
        return -1;

    rb->buffer[rb->head] = data;
    printf("Saving data %d at head %d\n", rb->buffer[rb->head], rb->head);
    rb->head = (rb->head + 1) % RING_BUFFER_MAX_SIZE;

    return 0;
}

int ring_buffer_pop(ring_buffer_t *const rb, uint8_t *data)
{
    if(!rb || !data)
        return -1;

    if(ring_buffer_is_empty(rb))
        return -1;

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUFFER_MAX_SIZE;

    return 0;
}

int ring_buffer_peek(ring_buffer_t * const rb, uint8_t *data)
{
    if(!rb || !data)
        return -1;

    if(ring_buffer_is_empty(rb))
        return -1;

    *data = rb->buffer[rb->tail];
    printf("Peek, head %d tail %d data %d\n",rb->head, rb->tail, *data);

    return 0;
}

int ring_buffer_is_full(ring_buffer_t * const rb)
{
    return (((rb->head + 1) % RING_BUFFER_MAX_SIZE) == rb->tail);
}

int ring_buffer_is_empty(ring_buffer_t *const rb)
{
    return (rb->head == rb->tail);
}

int ring_buffer_find(ring_buffer_t *const rb, uint8_t item)
{
    if(ring_buffer_is_empty(rb) || !rb)
        return -1;

    int index = rb->tail;
    for(int i = 0; i < ring_buffer_used(rb); ++i)
    {
        if(rb->buffer[i] == item)
            return index;

        index = (index + 1) % RING_BUFFER_MAX_SIZE;
    }
    // item not in buffer
    return -1;
}
