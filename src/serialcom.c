/*
 * This file implements the serial communication with the Arduino UNO board. 
 */

// Module include
#include "serialcom.h"

// project includes
#include "serial.h"
#include "buffer.h"

// system includes
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// serial handle
static serial_t *serialHandle_ = NULL;
// serial thread handle
static pthread_t serialListener_;
// serial read/write buffer
static Buffer_t *serialBuffer_ = NULL;
// serial buffer max size
#define SERIAL_BUFFER_SIZE 128
// size for command msg buffer
#define COMMAND_MSG_BUFFER_SIZE 64

// static status variable determining if serial listener thread running
static bool threadRunning_ = false;
// status if serial connection is established
static bool serialInitialized_ = false;

// Used baud rate for communication with arduino
static const uint32_t BAUD_RATE = 152000;
// TODO: set active serial port from outside!
static const char * const SERIAL_PATH = "/dev/pts/1";
// Used timeout for serial communication
static const uint8_t TIMEOUT = 100; /* ms */

/* ========== Serial Command/Response Values ========== */
// Must be identical to Arduino implementation

typedef enum command_
{
    /* "setup" commands */
    CMD_HELLO_THERE     = 0xB0, /* initial hello */
    CMD_HAPPY_LANDING   = 0xB1, /* end connection "nicely" */

    /* "normal" commands */
    CMD_HOMING_AXIS_X   = 0xC0, /* homing procedure X-axis */
    CMD_HOMING_AXIS_Y   = 0xC1, /* homing procedure Y-axis */
    CMD_MOVE            = 0xC2, /* move to position + up/down motion */
    CMD_STOP            = 0xC4, /* "emergency" stop */

    /* "optional" commands */
    CMD_CHANGE_POSITION = 0xCA, /* move to specific position without going up/down */
    CMD_FILL_GLASS      = 0xCB, /* only up and down motion TODO: Position check */

    /* error commands */
    CMD_ERROR           = 0xEE  /* TODO: specify errors */
} Command_t;

typedef enum response_
{
    RESP_CMD_DONE   = 0xA0, /* command was successful */
    RESP_CMD_STATUS = 0xA1, /* interim status of command */
    RESP_ERROR      = 0xA2  /* something went wrong on the Arduino */
} Response_t;

/* ========== SERIAL FRAME ========== */

enum FramePosition
{
    FRAME_POS_START_VAL   = 0,    /* frame starting position */
    FRAME_POS_CMD_VAL     = 1,    /* position of specific command */
    FRAME_POS_BYTE_COUNT  = 2,    /* length of command payload */
    FRAME_POS_DATA_START  = 3     /* start of command payload */
};

enum FrameValues
{
    FRAME_VAL_START         = 0x10, /* start of frame */
    FRAME_VAL_XOR           = 0x11, /* XOR char */
    FRAME_VAL_ESCAPE_CHAR   = 0x20  /* escape char */
};

/* ========== SERIAL PROTOCOL STATUS ========== */

typedef enum protocolStatus_
{
    STATE_IDLE = 1,     /* nothing happens */
    STATE_CMD_RSP,      /* decode command value */
    STATE_BYTE_COUNT,   /* number of data bytes */
    STATE_READ_DATA,    /* n bytes of data, e.g. position values */
    STATE_CHECKSUM,     /* check if message is valid */
    STATE_ERROR         /* Something went wrong, check error */
} ProtocolStatus_t;

/* ========== SERIAL PRIVATE FUNCTIONS ========== */

static uint16_t
serialcom_calculate_checksum(const uint8_t *buffer, size_t size)
{
    uint16_t sum = 0;
    for(size_t i = 0; i < size; ++i)
    {
        sum += buffer[i];
    }
    return sum;
}

/**
 *
 * @param buffer
 * @param pos
 * @param quantity
 * @return size of message
 */
static uint8_t
serialcom_construct_move_cmd(uint8_t *buffer, gint8 pos, guint quantity)
{
    memset(buffer, 0, COMMAND_MSG_BUFFER_SIZE);
    buffer[FRAME_POS_START_VAL] = FRAME_VAL_START;
    buffer[FRAME_POS_CMD_VAL] = CMD_HELLO_THERE;
    buffer[FRAME_POS_BYTE_COUNT] = 2; // pos + quantity
    buffer[FRAME_POS_DATA_START] = (uint8_t)pos;
    buffer[FRAME_POS_DATA_START+1] = (int8_t)quantity;
    uint8_t checksumPos = FRAME_POS_DATA_START + 2;

    uint16_t checksum = serialcom_calculate_checksum(buffer, FRAME_POS_DATA_START+1);
    buffer[checksumPos++] = (checksum >> 8) & 0xFF;
    buffer[checksumPos] = (checksum & 0xFF);
    // checksum position is number of bytes in buffer
    return checksumPos;
}

static int
serialcom_send_data(const uint8_t *buffer, size_t size)
{
    /* send serial data using serial handle */
    if(serial_write(serialHandle_, buffer, size) < 0)
    {
        fprintf(stderr, "SerialError: %s\n", serial_errmsg(serialHandle_));
        return serial_errno(serialHandle_);
    }
    return 0;
}

static int
serialcom_read_data(uint8_t *buffer, size_t size)
{
    int bytesRead = 0;
    /* read serial data into buffer */
    if((bytesRead = serial_read(serialHandle_, buffer, size, TIMEOUT)) < 0)
    {
        fprintf(stderr, "SerialError: Error reading serial msg!\n");
        return -1;
    }
    return bytesRead;
}

static uint8_t
serialcom_decode_response(const uint8_t *buffer, Response_t *resp)
{
    if(buffer == NULL)
    {
        fprintf(stderr, "SerialError: Recieve Buffer is NULL!\n");
        return -1;
    }

    ProtocolStatus_t decodeState = STATE_IDLE;
    bool finished = false;
    uint8_t numBytes = 0, checksum = 0, protocolIndex = 0;

    while(!finished)
    {
        switch(decodeState)
        {
            case STATE_IDLE:
            {
                uint8_t startVal = buffer[protocolIndex++];
                if(startVal != FRAME_VAL_START)
                {
                    decodeState = STATE_ERROR;
                    break;
                }
                checksum += startVal;
                decodeState = STATE_CMD_RSP;
                break;
            }
            case STATE_CMD_RSP:
            {
                *resp = buffer[protocolIndex++];
                checksum += *resp;

                decodeState = STATE_BYTE_COUNT;
                break;
            }
            case STATE_BYTE_COUNT:
            {
                numBytes = buffer[protocolIndex++];
                if(numBytes > 0)
                {
                    checksum += numBytes;
                    decodeState = STATE_READ_DATA;
                    break;
                }
                decodeState = STATE_ERROR;
                break;
            }
            case STATE_READ_DATA:
            {
                for(int i = 0; i < numBytes; ++i)
                {
                    checksum += buffer[protocolIndex++];
                }
                decodeState = STATE_CHECKSUM;
                break;
            }
            case STATE_CHECKSUM:
            {
                if(checksum == buffer[protocolIndex])
                {
                    finished = true;
                    break;
                }
                decodeState = STATE_ERROR;
                break;
            }
            case STATE_ERROR:
            {
                fprintf(stderr, "SerialError: Error during communication!\n");
                return -1;
            }
        }
    }

    return 0;
}

// worker thread running until program exits
static void *
serialcom_thread_func(void *arg)
{
    Buffer_t *buffer = (Buffer_t *)arg;

    uint8_t rcvBuffer[128];
    memset(rcvBuffer, 0, 128);
    int bytesRecieved = 0;
    Response_t response;

    while(threadRunning_)
    {
        if(buffer_read_try(buffer, rcvBuffer, 128) != BUFFER_EMPTY)
        {
            printf("Sending msg: %s\n", rcvBuffer);
            serialcom_send_data(rcvBuffer, 128);
            memset(rcvBuffer, 0, 128);
        }
        if(serialInitialized_ && (bytesRecieved = serialcom_read_data(rcvBuffer, 128)) > 0)
        {
            serialcom_decode_response(rcvBuffer, &response);
            memset(rcvBuffer, 0, 128);
        }
    }

    // TODO: Find something to return!
    return NULL;
}

/* ========== SERIAL THREAD PUBLIC FUNCTIONS ========== */

// start the serial handler thread
// this thread sends and listens for serial data
int serialcom_start_handler_thread()
{
    serialBuffer_ = buffer_new(SERIAL_BUFFER_SIZE);
    if(serialBuffer_ == NULL)
        return -1;

    threadRunning_ = true;
    return pthread_create(&serialListener_,
            NULL,
            serialcom_thread_func,
            (void *) serialBuffer_);    // pass buffer to thread
}

// stop running serial thread
void serialcom_stop_handler_thread()
{
    threadRunning_ = false;
    // TODO: Cancel or join thread?
    pthread_join(serialListener_, NULL);
    buffer_destroy(serialBuffer_);
}

/* ========== SERIAL COMMUNICATION PUBLIC FUNCTIONS ========== */

/**
 * @brief Allocate new serial handler and open serial port
 * @return 0 if port is open, error otherwise
 */
int
serialcom_initialize_connection()
{
    /* allocate new serial handle for duration of serial listener thread */
    if((serialHandle_ = serial_new()) == NULL)
    {
        fprintf(stderr, "SerialError: Could not allocate serial handle\n");
        return SERIAL_ERROR_ARG;
    }
    /* Open serial port with path and baud rate */
    if(serial_open(serialHandle_, SERIAL_PATH, BAUD_RATE) < 0)
    {
        fprintf(stderr, "SerialError: %s\n", serial_errmsg(serialHandle_));
        return serial_errno(serialHandle_);
    }

    /* Success */
    serialInitialized_ = true;
    return 0;
}

/**
 * @brief Close serial port (free serial handle?)
 * @return 0 if port is closed, error otherwise
 */
int
serialcom_cancel_connection()
{
    if(serial_close(serialHandle_) < 0)
    {
        fprintf(stderr, "SerialError: %s\n", serial_errmsg(serialHandle_));
        return serial_errno(serialHandle_);
    }

    serial_free(serialHandle_);
    /* Success */
    return 0;
}

int
serialcom_start_order(URecipe *rec)
{
    if(rec == NULL)
    {
        fprintf(stderr, "SerialError: Recipe object is NULL!\n");
        return -1;
    }

    uint32_t ingCount = u_recipe_get_ingredient_count(rec);
    // position and quantity of ingredient for serial command
    int8_t pos = 0;
    uint32_t quantity = 0;
    uint8_t msgSize = 0;
    uint8_t msgBuffer[COMMAND_MSG_BUFFER_SIZE];

    // TODO: send each move as individual command or send everything together ??
    for(uint16_t i = 0; i < ingCount; ++i)
    {
        /* Construct message */
        if(!u_recipe_get_nth_ingredient_info(rec, i, &pos, &quantity))
            return -1;
        if((msgSize = serialcom_construct_move_cmd(msgBuffer, pos, quantity)) <= 0)
            return -1;
        /* Write to buffer -> serial thread reads msg and sends it */
        if(buffer_write_wait(serialBuffer_, msgBuffer, msgSize) != BUFFER_WRITE_OK)
            return -1;
    }
    return 0;
}
