#include "util/dynamic_array.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define VLA_INITIAL_SIZE 5
#define RESIZE_FACTOR 1.6f

#define ELEM_AT(a, N) (a->data + (a->elemSize * N))

void vla_init(VLArray_t *array, size_t elemSize)
{
    array->data = calloc(VLA_INITIAL_SIZE, elemSize);
    array->elemSize = elemSize;
    array->used = 0;
    array->size = VLA_INITIAL_SIZE;
}

void vla_free(VLArray_t *array)
{
    if(!array)
        return;

    if(array->data != NULL)
    {
        free(array->data);
        array->data = NULL;
    }
}

int vla_append(VLArray_t *array, void *data)
{
    if(!array || !data)
        return 1;
    
    if(array->size == array->used)
    {
        const size_t newSize = ceil(array->size * RESIZE_FACTOR);
        array->data = realloc(array->data, newSize * array->elemSize);
        array->size = newSize;
    }
/* Ignore pointer arithmetic warning, we know what we are doing */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
    void *last = ELEM_AT(array, array->used);
#pragma GCC diagnostic pop

    memcpy(last, data, array->elemSize);
    array->used++;

    return 0;
}

void *vla_get_elem_at(VLArray_t *array, uint32_t index)
{
    if(!array)
        return NULL;

/* Ignore pointer arithmetic warning, we know what we are doing */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
    void *elem = ELEM_AT(array, index);
#pragma GCC diagnostic pop
    return elem;
}
