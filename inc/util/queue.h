#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#define CMD_BUFFER_SIZE 128

struct CommandBuffer
{
    uint16_t id;
    uint16_t cmd;
    uint8_t data[CMD_BUFFER_SIZE];
};

typedef struct queue
{
    int head;
    int tail;

    struct CommandBuffer *commands;
} command_queue_t;

int cmd_queue_init(command_queue_t *cq);

void cmd_queue_free(command_queue_t *cq);

void cmd_queue_print(command_queue_t *cq);

int cmd_queue_push(command_queue_t * const cq, const struct CommandBuffer *buf);

int cmd_queue_pop(command_queue_t * const cq, struct CommandBuffer * const buf);

#endif // QUEUE_H
