#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "util/cmd_buffer.h"

typedef struct cmd_msg
{
    uint8_t id;
    cmd_buffer_t cmdBuffer;
    uint16_t checksum;
} cmd_msg_t;

int message_handler_init(void);

int message_handler_deinit(void);

int message_handler_add_incoming(const cmd_msg_t msg);

int message_handler_add_outgoing(const cmd_buffer_t cmdBuffer);

void message_handler_process(void);

#endif // MESSAGE_HANDLER_H
