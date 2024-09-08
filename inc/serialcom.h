#ifndef __SERIALCOM_H__
#define __SERIALCOM_H__

#include "recipe.h"

/* serial thread functions */

int serialcom_start_handler_thread();

void serialcom_stop_handler_thread();

/* serial communication functions */

int serialcom_initialize_connection();

int serialcom_cancel_connection();

int serialcom_start_order(URecipe *rec);

#endif // __SERIALCOM_H__
