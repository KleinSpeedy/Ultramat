/*
 * This file implements the serial communication with the Arduino UNO board. 
 */

// Module include
#include "serialcom.h"
#include "serial.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// serial handle
static serial_t *serialHandle;
static pthread_t serialListener;

static bool threadRunning = false;

static uint8_t
serialcom_calculate_checksum()
{
   return 0; 
}

static void *
serialcom_thread_func()
{
    while(threadRunning)
    {
    }
}

// start the serial handler thread
// this thread sends and listens for serial data
int serialcom_start_handler_thread()
{
    threadRunning = true;
    int ret = pthread_create(&serialListener,
                NULL,
                serialcom_thread_func,
                NULL);
    return ret;
}

void serialcom_stop_handler_thread()
{
    threadRunning = false;
    pthread_join(serialListener, NULL);
}

/* serial communication */

int serialcom_initialize_connection()
{
    serialHandle = serial_new();
    uint8_t recvBuffer[128];
    int ret;

    // TODO: set active serial port from outside!
    if(serial_open(serialHandle, "/dev/ttyACM0", 115200) < 0)
    {
        fprintf(stderr, "Error opening serial connection!\n");
        serial_free(serialHandle);
        return 1;
    }

    memset(recvBuffer, 0, sizeof(recvBuffer));
    // test data
    const uint8_t txBuffer[] = {0x10, 0x14, 0x01, 0x01, 0x26};

    if(serial_write(serialHandle, txBuffer, sizeof(txBuffer)) < 0)
    {
        fprintf(stderr, "Error writing serial data!\n");
        serial_free(serialHandle);
        serial_close(serialHandle);
        return 2;
    }

    if((ret = serial_read(serialHandle, recvBuffer, sizeof(recvBuffer), 100)) < 0)
    {
       fprintf(stderr, "Error reading serial data!\n");
       serial_free(serialHandle);
       serial_close(serialHandle);
       return 3;
    }
    
    printf("Read: %x\n", ret);

    serial_close(serialHandle);
    serial_free(serialHandle);
    return 0;
}

int serialcom_cancel_connection()
{
    return 0;
}
