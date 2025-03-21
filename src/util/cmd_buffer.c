#include "util/cmd_buffer.h"

#include <stdio.h>

/* command buffer functions */

void cmd_buffer_print(const cmd_buffer_t *cmdBuffer)
{
    printf("CMD: %d, Data Size: %d, Payload: ", cmdBuffer->cmd, cmdBuffer->size);
    for(int i = 0; i < cmdBuffer->size; i++)
    {
        printf("%02X ", cmdBuffer->data[i]);
    }
    printf("\n");
}
