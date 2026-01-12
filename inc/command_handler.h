#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "proto/commands.pb.h"

void command_handler_init(void);

int command_handler_add(const uint8_t id, const CmdType cmd);

int command_handler_execute_done(const Response *resp);

#endif // COMMAND_HANDLER_H
