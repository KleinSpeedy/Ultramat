#include "util/queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_SIZE 32

static inline int _cmd_queue_is_full(command_queue_t *q)
{
    return (q->tail + 1) % QUEUE_SIZE == q->head;
}

static inline int _cmd_queue_is_empty(command_queue_t *q)
{
    return (q->head == -1);
}

int cmd_queue_init(command_queue_t *cq)
{
    cq->tail = -1;
    cq->head = -1;

    cq->commands = calloc(QUEUE_SIZE, sizeof(*cq->commands) * QUEUE_SIZE);
    if(!cq->commands)
        return -1;

    return 0;
}

void cmd_queue_free(command_queue_t *cq)
{
    if(!cq)
        return;

    if(cq->commands != NULL)
    {
        free(cq->commands);
        cq->commands = NULL;
    }
}

void cmd_queue_print(command_queue_t *cq)
{
    for(int i = 0; i < QUEUE_SIZE; i++)
    {
        const struct CommandBuffer temp = cq->commands[i];
        printf("%d %d %s\n", temp.id, temp.cmd, temp.data);
    }
}

int cmd_queue_push(command_queue_t * const cq, const struct CommandBuffer *cmdBuffer)
{
    if(!cq || !cmdBuffer)
        return -1;

    if(_cmd_queue_is_full(cq))
        return -1;

    // first push
    if(_cmd_queue_is_empty(cq))
        cq->head = 0;

    memcpy(&cq->commands[cq->head], cmdBuffer, sizeof(*cmdBuffer));
    cq->head = (cq->head + 1) % QUEUE_SIZE;
    return 0;
}

int cmd_queue_pop(command_queue_t * const cq, struct CommandBuffer * const cb)
{
    if(!cq)
        return -1;

    if(_cmd_queue_is_empty(cq))
        return -1;

    // first pop of data
    if(cq->tail == -1)
        cq->tail = 0;

    memcpy(cb, &cq->commands[cq->tail], sizeof(*cb));
    // clear after copy
    memset(&cq->commands[cq->tail], 0, sizeof(*cb));
    cq->tail = (cq->tail + 1) % QUEUE_SIZE;
    return 0;
}
