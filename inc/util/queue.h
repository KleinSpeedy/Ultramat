#ifndef QUEUE_H
#define QUEUE_H

#include "message_handler.h"

#define QUEUE_MAX_SIZE 32

typedef struct queue
{
    int head;
    int tail;

    // TODO: Change to static array
    cmd_msg_t *commands;
} command_queue_t;

int cmd_queue_init(command_queue_t *cq);

void cmd_queue_free(command_queue_t *cq);

void cmd_queue_print(command_queue_t *cq);

int cmd_queue_push(command_queue_t * const cq, const cmd_msg_t*buf);

int cmd_queue_pop(command_queue_t * const cq, cmd_msg_t * const buf);

int cmd_queue_is_empty(command_queue_t * const q);

#endif // QUEUE_H
