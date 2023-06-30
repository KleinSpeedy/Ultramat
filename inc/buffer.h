/**
 * Header file for custom buffer implementation
 */

#ifndef __ULTRAMAT_BUFFER_H__
#define __ULTRAMAT_BUFFER_H__

/* Includes */
#include <pthread.h>
#include <stdint.h>

typedef struct _buffer
{
    pthread_mutex_t lock;   /* make buffer "thread safe" */

    int write;              /* current write position */
    int read;               /* current read position */

    uint8_t *data;             /* actual buffer content */
    int size;               /* actual buffer size */
} Buffer_t;

typedef enum _buf_status
{
    BUFFER_ERROR = -1,
    BUFFER_WRITE_OK,    /* */
    BUFFER_READ_OK,     /* Successfully read elements */
    BUFFER_FULL         /* Read == Write position */
} Buf_Status_t;

Buffer_t * buffer_new(int size);
void buffer_destroy(Buffer_t *buf);

Buf_Status_t buffer_write_try(Buffer_t *buf, uint8_t *data, int count);
Buf_Status_t buffer_write_wait(Buffer_t *buf, uint8_t *data, int count);

Buf_Status_t buffer_read(Buffer_t *buf, uint8_t *readBuf, int count);

#endif //__ULTRAMAT_BUFFER_H__
