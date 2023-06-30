/**
* Source implementation of custom serial buffer
*/

/* Module include */
#include "buffer.h"

/* System includes */
#include <stdlib.h>

/* Thread locking status */
typedef enum lockstatus_
{
    LOCK_SUCCESS = 0,
    LOCK_USED
} LocKStatus_t;

/* ========== STATIC FUNCTIONS ========== */

static inline void
buffer_lock(Buffer_t *buf)
{
    pthread_mutex_lock(&buf->lock);
}

static inline void
buffer_unlock(Buffer_t *buf)
{
    pthread_mutex_unlock(&buf->lock);
}

static inline LocKStatus_t
buffer_try_lock(Buffer_t *buf)
{
    if(pthread_mutex_trylock(&buf->lock))
        return LOCK_USED;
    return LOCK_SUCCESS;
}

static inline int
buffer_empty(Buffer_t *buf)
{
    return (buf->write == buf->read);
}

/**
 * @brief Create a new buffer of passed size that can hold commands and responses
 * @param size size of elements that buffer can hold
 * @return newly allocated object, NULL on failure
 */
Buffer_t *
buffer_new(int size)
{
    Buffer_t *buffer = (Buffer_t *) calloc(1, sizeof(Buffer_t));
    if(buffer == NULL)
        return NULL;

    pthread_mutex_init(&buffer->lock, NULL);
    buffer->read = 0;
    buffer->write = 0;

    buffer->size = size;
    buffer->data = (uint8_t *) calloc(buffer->size, sizeof(uint8_t));
    if(buffer->data == NULL)
    {
        free(buffer);
        return NULL;
    }

    return buffer;
}

/**
 * @brief Destroy the buffer with all members
 * @param buf buffer to destroy
 */
void buffer_destroy(Buffer_t *buf)
{
    if(buf == NULL)
        return;

    pthread_mutex_destroy(&buf->lock);
    free(buf->data);
    free(buf);
}

/**
 * @brief Try writing to the buffer and return if buffer is currently used -> NON_BLOCKING
 * @param buf serial buffer
 * @param data temporary write buffer from calling context
 * @param count number of elements that should be written
 * @return BUFFER_USED if lock could not be obtained, ERROR/WRITE_OK otherwise
 */
Buf_Status_t buffer_write_try(Buffer_t *buf, const uint8_t *data, int count)
{
    if(buf == NULL || data == NULL)
        return BUFFER_ERROR;

    /* Return if buffer is currently used */
    if(buffer_try_lock(buf) == LOCK_USED)
    {
        return BUFFER_USED;
    }
    /* Copy data from temp buffer to actual buffer */
    for(int i = 0; i < count; ++i)
    {
        buf->data[buf->write] = data[i];
        buf->write += 1 % buf->size;    // wrap around
    }
    buffer_unlock(buf);
    return BUFFER_WRITE_OK;
}

/**
 * @brief Try writing to the buffer and wait if buffer is currently used -> BLOCKING
 * @param buf serial buffer
 * @param data temporary write buffer from calling context
 * @param count number of elements that should be written
 * @return BUFFER_WRITE_OK if elems could be written to the buffer, ERROR otherwise
 */
Buf_Status_t buffer_write_wait(Buffer_t *buf, const uint8_t *data, int count)
{
    if(buf == NULL || data == NULL)
        return BUFFER_ERROR;

    /* Wait until lock is obtained */
    buffer_lock(buf);
    /* Copy data from temp buffer to actual buffer */
    for(int i = 0; i < count; ++i)
    {
        buf->data[buf->write] = data[i];
        buf->write += 1 % buf->size;    // wrap around
    }
    buffer_unlock(buf);
    return BUFFER_WRITE_OK;
}

/**
 * @brief Try reading from the buffer and return if buffer is currently used -> NON_BLOCKING
 * @param buf serial buffer
 * @param readBuf temporary write buffer from calling context
 * @param count number of elements that should be written
 * @return BUFFER_USED if lock could not be obtained, ERROR/READ_OK otherwise
 */
Buf_Status_t buffer_read_try(Buffer_t *buf, uint8_t *readBuf, int count)
{
    if(buf == NULL || readBuf == NULL)
        return BUFFER_ERROR;

    if(buffer_empty(buf))
        return BUFFER_EMPTY;

    /* Return if buffer is currently used */
    if(buffer_try_lock(buf) == LOCK_USED)
    {
        return BUFFER_USED;
    }
    /* Copy data from temp buffer to actual buffer */
    for(int i = 0; i < count && buf->read < buf->write; ++i)
    {
        readBuf[i] = buf->data[buf->read];
        buf->read += 1 % buf->size;    // wrap around
    }
    buffer_unlock(buf);
    return BUFFER_READ_OK;
}

/**
 * @brief Try reading from the buffer and wait if buffer is currently used -> BLOCKING
 * @param buf serial buffer
 * @param readBuf temporary write buffer from calling context
 * @param count number of elements that should be written
 * @return BUFFER_USED if lock could not be obtained, ERROR/READ_OK otherwise
 */
Buf_Status_t buffer_read_wait(Buffer_t *buf, uint8_t *readBuf, int count)
{
    if(buf == NULL || readBuf == NULL)
        return BUFFER_ERROR;

    if(buffer_empty(buf))
        return BUFFER_EMPTY;

    /* Wait until lock is obtained */
    buffer_lock(buf);
    /* Copy data from temp buffer to actual buffer */
    for(int i = 0; i < count && buf->read < buf->write; ++i)
    {
        readBuf[i] = buf->data[buf->read];
        buf->read += 1 % buf->size;    // wrap around
    }
    buffer_unlock(buf);
    return BUFFER_READ_OK;
}
