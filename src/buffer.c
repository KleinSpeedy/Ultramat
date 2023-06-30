/**
* Source implementation of custom serial buffer
*/

/* Module include */
#include "buffer.h"

/* System includes */
#include <stdlib.h>

Buffer_t *
buffer_new(int size)
{
    Buffer_t *buffer = (Buffer_t *) calloc(1, sizeof(Buffer_t));
    buffer->read = 0;
    buffer->write = 0;

    buffer->size = size;
    buffer->data = (uint8_t *) calloc(buffer->size, sizeof(uint8_t));

    return buffer;
}

void buffer_destroy(Buffer_t *buf)
{
    if(buf == NULL)
        return;

    free(buf->data);
    free(buf);
}

Buf_Status_t buffer_write_try(Buffer_t *buf, uint8_t *data, int count)
{

}

Buf_Status_t buffer_write_wait(Buffer_t *buf, uint8_t *data, int count)
{

}

Buf_Status_t buffer_read(Buffer_t *buf, uint8_t *readBuf, int count)
{

}
