#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "util/cmd_buffer.h"

void command_handler_init(void);

int command_handler_add(const uint8_t id, const eCommand cmd);

int command_handler_execute_done(const uint8_t id, const uint8_t *payload,
                                 const uint8_t size);

#endif // COMMAND_HANDLER_H
