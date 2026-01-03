#ifndef PROTO_SERIAL_H
#define PROTO_SERIAL_H

#include "drinks.h"
#include "proto/commands.pb.h"

int proto_comms_start_serial_connection(void);

void proto_comms_stop_serial_connection(void);

int proto_comms_start_new_mixing(const Recipe *rec);

int proto_comms_send_msg(const Message *msg);

bool proto_comms_thread_running(void);

#endif // PROTO_SERIAL_H
