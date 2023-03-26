#ifndef __SERIALCOM_H__
#define __SERIALCOM_H__

/* serial thread functions */

int serialcom_start_handler_thread();

void serialcom_stop_handler_thread();

/* serialcom functions */
int serialcom_initialize_connection();

int serialcom_cancel_connection();

#endif // __SERIALCOM_H__
