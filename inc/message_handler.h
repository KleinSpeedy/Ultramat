#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "proto/commands.pb.h"

int message_handler_init(void);
int message_handler_deinit(void);

int message_handler_add_incoming(const Response *resp);
int message_handler_add_outgoing(Message *const msg);

int message_handler_move_x(const PositionX pos);
int message_handler_move_y(const uint8_t count);

int message_handler_process(void);

#endif // MESSAGE_HANDLER_H
