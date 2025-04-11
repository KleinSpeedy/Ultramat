#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include "drinks.h"
#include "message_handler.h"

// start, id, cmd, data size, data (1 .. n), checksum MSB, checksum LSB
#define COMMS_MIN_MSG_SIZE      7

// minimum message size but we remove the needed data byte so we can add the
// payload size manually
#define COMMS_MIN_MSG_SIZE_NO_PAYLOAD   (COMMS_MIN_MSG_SIZE - 1)

// Maximum size of a buffer in bytes is the maximum payload size + minimum
// message size
#define COMMS_MAX_BUFFER_SIZE   (CMD_BUFFER_SIZE + COMMS_MIN_MSG_SIZE)

int comms_start_new_mixing(Recipe *rec);

int comms_send_msg(const cmd_msg_t *msg);

int comms_start_serial_connection(void);

void comms_stop_serial_connection(void);

#endif // SERIALCOMMS_H
