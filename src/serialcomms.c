#include "serialcomms.h"
#include "callbacks_position.h"
#include "serial.h"
#include "util/cmd_buffer.h"
#include "message_handler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <termios.h>

typedef struct PositionCountPair
{
    ComboPositions_t pos;
    uint16_t count;
} PositionCountPair_t;

typedef enum eMsgDecodingState
{
    DECODE_IDLE = 0,
    DECODE_ID,
    DECODE_CMD,
    DECODE_DATA_LENGTH,
    DECODE_DATA,
    DECODE_CHECKSUM,

    DECODE_ERROR
} eMsgDecodingState;

// serial handle
static serial_t *ser_ = NULL;
#define FREE_SERIAL_HANDLE(x)   serial_free(x); x = NULL
// TODO: Set this differently
#define SERIAL_PORT     "/tmp/ttyBase"

// thread handle
static pthread_t tinfo_;
static bool threadRunning_ = false;

/* static function declarations */

static void *comms_serial_thread_func(void *arg);
static int comms_start_new_mixing_impl(PositionCountPair_t *pc, uint16_t count);
static int comms_read_message(cmd_msg_t *temp);
static int comms_decode_message(const uint8_t *buffer, const uint8_t size,
                                cmd_msg_t *temp);

/* public functions */

int comms_start_new_mixing(Recipe *rec)
{
    if(!rec || !threadRunning_)
        return -1;

    PositionCountPair_t pc[rec->ingCount];
    memset(&pc, 0, sizeof(*pc) * rec->ingCount);

    for(uint16_t i = 0; i < rec->ingCount; i++)
    {
        const ComboPositions_t pos = cb_get_position_by_id(rec->ingPairs[i].id);
        if(pos < PAGES_COMBO_POS_ONE || pos >= PAGES_COMBO_POS_NUM)
            return -1;

        pc[i].pos = pos;
        pc[i].count = rec->ingPairs[i].count;
    }

    return comms_start_new_mixing_impl(pc, rec->ingCount);
}

int comms_start_serial_connection(void)
{
    // allocate serial handle
    if((ser_ = serial_new()) == NULL)
    {
        // TODO: Bad alloc
        return -1;
    }
    // setup serial connection
    if(serial_open(ser_, SERIAL_PORT, 115200) < 0)
    {
        // TODO: Better error log
        fprintf(stderr, "serial_open(): %s\n", serial_errmsg(ser_));
        FREE_SERIAL_HANDLE(ser_);
        return -1;
    }
    // clear serial port buffer before ending
    tcflush(serial_fd(ser_), TCIFLUSH);

    // init command queues
    if(message_handler_init() < 0)
    {
        fprintf(stderr, "Error initialising message_handler\n");
        serial_close(ser_);
        FREE_SERIAL_HANDLE(ser_);
        return -1;
    }
    // start serial thread func
    if(pthread_create(&tinfo_, NULL, comms_serial_thread_func, NULL) != 0)
    {
        fprintf(stderr, "Error creating pthread\n");
        FREE_SERIAL_HANDLE(ser_);
        return -1;
    }

    return 0;
}

void comms_stop_serial_connection(void)
{
    // TODO: is this even possible?
    if(!threadRunning_)
        return;
    threadRunning_ = false;

    // wait for serial thread to finish
    // TODO: Is this enough or do we need cond variable?
    pthread_join(tinfo_, NULL);
    message_handler_deinit();
    serial_close(ser_);
    FREE_SERIAL_HANDLE(ser_);
}

int comms_send_msg(const cmd_msg_t *msg)
{
    if(!ser_)
        return -1;

    // number of bytes without checksum
    const uint8_t len = 4 + msg->cmdBuffer.size;
    if((len + 2) > COMMS_MAX_BUFFER_SIZE)
        return -1;

    uint8_t buffer[COMMS_MAX_BUFFER_SIZE] = {0};

    buffer[FRAME_POS_START] = FRAME_VAL_START;
    buffer[FRAME_POS_ID] = msg->id;
    buffer[FRAME_POS_CMD] = msg->cmdBuffer.cmd;
    buffer[FRAME_POS_DATA_LENGTH] = msg->cmdBuffer.size;
    memcpy(&buffer[FRAME_POS_DATA], msg->cmdBuffer.data, msg->cmdBuffer.size);
    buffer[len] = (msg->checksum >> 8) & 0xFF;
    buffer[len+1] = msg->checksum & 0xFF;

    int numBytes = 0;
    if((numBytes = serial_write(ser_, buffer, len + 2)) < 0)
        return -1;

    return numBytes;
}

/* static function definitions */

static int comms_start_new_mixing_impl(PositionCountPair_t *pc, uint16_t count)
{
    for(int i = 0; i < count; ++i)
    {
        const cmd_buffer_t next = CMD_BUFFER_MOVE(pc[i].pos, pc[i].count);
        if(message_handler_add_outgoing(next) < 0)
        {
            return -1;
        }
    }

    return 0;
}

static void *comms_serial_thread_func(void *arg)
{
    (void) arg;

    // add setup command as first cmd buffer
    message_handler_add_outgoing(CMD_BUFFER_HELLO_THERE);

    //uint32_t incoming = 0;
    threadRunning_ = true;
    while(threadRunning_)
    {
        if(serial_poll(ser_, 0 ))
        {
            cmd_msg_t temp = {0};
            if(comms_read_message(&temp) < 0)
            {
                // TODO: Error log
                printf("Error reading message\n");
            }
            if(message_handler_add_incoming(temp) < 0)
            {
                printf("Error appending incoming msg\n");
            }
        }

        message_handler_process();
    }

    return NULL;
}

static int comms_read_message(cmd_msg_t *temp)
{
    uint8_t inBuffer[COMMS_MAX_BUFFER_SIZE] = {0};
    uint8_t numBytes = 0;
    if((numBytes = serial_read(ser_, inBuffer, sizeof(inBuffer), 0)) < 1)
    {
        // TODO: Error log
        printf("Fail of serial read\n");
        return -1;
    }

    int ret = 0;
    if((ret = comms_decode_message(inBuffer, numBytes, temp)) < 0)
    {
        // TODO: Error log
        printf("Fail of decode message: %d\n", ret);
        return -1;
    }

    return 0;
}

static int comms_decode_message(const uint8_t *buffer, const uint8_t numBytes,
                                cmd_msg_t *temp)
{
    // TODO: 5 magic value, is it correct though?
    if(numBytes < 5 || buffer[FRAME_POS_START] != FRAME_VAL_START)
    {
        return -1;  // Invalid frame
    }

    const uint8_t id = buffer[FRAME_POS_ID];
    const uint8_t cmd = (eCommand)buffer[FRAME_POS_CMD];
    const uint8_t dataLength = buffer[FRAME_POS_DATA_LENGTH];

    // TODO: Why 5???
    if(numBytes < (5 + dataLength))
    {
        return -2; // Incomplete frame
    }

    const uint8_t upperChecksumByte = 
        (buffer[FRAME_POS_DATA + dataLength] >> 8) & 0xFF;
    const uint8_t lowerChecksumByte = 
        buffer[FRAME_POS_DATA + dataLength + 1] & 0xFF;

    const uint16_t recievedChecksum = upperChecksumByte + lowerChecksumByte;

    uint16_t calculatedChecksum = 0;
    for(uint8_t i = 0; i < (numBytes - 2); ++i)
    {
        calculatedChecksum += buffer[i];
    }
    if(calculatedChecksum != recievedChecksum)
    {
        // TODO: Error log
        return -3; // checksum mismatch
    }

    temp->id = id;
    // TODO: Check if valid command val
    temp->cmdBuffer.cmd = cmd;
    temp->cmdBuffer.size = dataLength;
    memcpy(temp->cmdBuffer.data, &buffer[FRAME_POS_DATA_LENGTH + 1], dataLength);
    temp->checksum = calculatedChecksum;

    return 0;
}
