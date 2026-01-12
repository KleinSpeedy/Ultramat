#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>
#include <stdint.h>

typedef struct VLArray
{
    void *data;      // element data
    size_t elemSize; // size of elements in bytes

    uint32_t size; // array size
    uint32_t used; // number of elements in array
} VLArray_t;

typedef void(custom_free_t(void *));

void vla_init(VLArray_t *array, size_t elemSize);

void vla_free(VLArray_t *array, custom_free_t free_func);

int vla_append(VLArray_t *array, void *data);

void *vla_get_elem_at(VLArray_t *array, uint32_t index);

#endif // DYNAMIC_ARRAY_H
