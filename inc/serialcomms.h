#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include "drinks.h"
#include "message_handler.h"

// Maximum size of a buffer in bytes is the maximum payload size + 6
#define COMMS_MAX_BUFFER_SIZE CMD_BUFFER_SIZE + 6

int comms_start_new_mixing(Recipe *rec);

int comms_send_msg(const cmd_msg_t *msg);

int comms_start_serial_connection(void);

void comms_stop_serial_connection(void);

#endif // SERIALCOMMS_H
