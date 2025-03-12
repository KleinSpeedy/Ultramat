#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include "drinks.h"

int comms_start_new_mixing(Recipe *rec);

int comms_start_serial_connection(void);

#endif // SERIALCOMMS_H
