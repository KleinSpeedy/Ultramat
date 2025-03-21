#include "util/queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int _cmd_queue_is_full(command_queue_t * const q)
{
    return (q->head + 1) % QUEUE_MAX_SIZE == q->tail;
}

int cmd_queue_is_empty(command_queue_t * const q)
{
    return (q->tail == q->head);
}

// TODO: Change to static alloc with QUEUE_MAX_SIZE known
int cmd_queue_init(command_queue_t *cq)
{
    cq->tail = 0;
    cq->head = 0;

    cq->commands = calloc(QUEUE_MAX_SIZE, sizeof(*cq->commands) * QUEUE_MAX_SIZE);
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
    // TODO: Prints all elements even if 0
    for(int i = 0; i < QUEUE_MAX_SIZE; i++)
    {
        const cmd_msg_t temp = cq->commands[i];
        printf("%d %d %s\n", temp.id, temp.cmdBuffer.cmd, temp.cmdBuffer.data);
    }
}

int cmd_queue_push(command_queue_t * const cq, const cmd_msg_t *msg)
{
    if(!cq || !msg)
        return -1;

    if(_cmd_queue_is_full(cq))
        return -1;

    memcpy(&(cq->commands[cq->head]), msg, sizeof(*msg));
    cq->head = (cq->head + 1) % QUEUE_MAX_SIZE;
    return 0;
}

int cmd_queue_pop(command_queue_t * const cq, cmd_msg_t * const msg)
{
    if(!cq)
        return -1;

    if(cmd_queue_is_empty(cq))
        return -1;

    memcpy(msg, &cq->commands[cq->tail], sizeof(*msg));
    // TODO: clear after copy ?
    // memset(&cq->commands[cq->tail], 0, sizeof(*msg));
    cq->tail = (cq->tail + 1) % QUEUE_MAX_SIZE;
    return 0;
}
