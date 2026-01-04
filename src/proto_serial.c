#include "proto_serial.h"
#include "callbacks_position.h"
#include "message_handler.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "proto/commands.pb.h"

#include <linux/prctl.h>
#include <pthread.h>
#include <serial.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <termios.h>

// serial handle
static serial_t *ser_ = NULL;
#define FREE_SERIAL_HANDLE(x)                                                  \
    serial_free(x);                                                            \
    x = NULL

// TODO: Set this differently
#ifndef SERIAL_PORT
#define SERIAL_PORT "/dev/ttyACM0"
#endif

#define SERIAL_BAUD 115200

// thread handle
static pthread_t tinfo_;
static bool threadRunning_ = false;

static void *proto_comms_serial_thread_func(void *arg);

static size_t proto_comms_msg_to_buffer(const Message *msg, uint8_t *buffer,
                                        const size_t size)
{
    if(!msg || !buffer)
        return -1;

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    const bool valid = pb_encode(&stream, Message_fields, msg);
    if(!valid)
        return -1;

    return stream.bytes_written;
}

static int proto_comms_read_response(Response *resp)
{
    if(!resp)
        return -1;

    uint8_t buffer[Response_size] = {0};

    const int numBytes = serial_read(ser_, buffer, Response_size, 0);
    if(numBytes < 0)
    {
        // TODO: Error log
        printf("Fail of serial read\n");
        return -1;
    }

    pb_istream_t stream =
        pb_istream_from_buffer((const uint8_t *)&buffer, numBytes);

    if(!pb_decode(&stream, Response_fields, resp))
        return -1;

    return numBytes;
}

/* public functions */

bool proto_comms_thread_running(void)
{
    return threadRunning_;
}

// TODO: This should be in message handler
int proto_comms_start_new_mixing(const Recipe *rec)
{
    if(!rec)
        return -1;

    for(uint16_t i = 0; i < rec->ingCount; ++i)
    {
        const IdCountPair pair = rec->ingPairs[i];
        const ComboPositions_t pos = cb_get_position_by_id(pair.id);
        if(pos == PAGES_COMBO_POS_INVALID)
            return -1;

        Message out = Message_init_zero;
        out.type = CmdType_RECIPE_STEP;
        out.which_payload = Message_step_tag;
        out.payload.step.xPos = (PositionX)(pos + 1);
        out.payload.step.yCount = pair.count;

        const int ret = message_handler_add_outgoing(&out);
        if(ret < 0)
            return -1;
    }

    Message final = Message_init_zero;
    final.type = CmdType_RECIPE_STEP;
    final.which_payload = Message_step_tag;
    final.payload.step.xPos = PositionX_FINAL;
    final.payload.step.yCount = 0;
    if(message_handler_add_outgoing(&final) < 0)
        return -1;

    return 1;
}

int proto_comms_send_msg(const Message *msg)
{
    uint8_t buffer[Message_size] = {0};

    const int len = proto_comms_msg_to_buffer(msg, buffer, Message_size);
    if(len < 0)
        return -1;

    if(serial_write(ser_, buffer, len) < 0)
        return -1;

    serial_flush(ser_);
    return 1;
}

int proto_comms_start_serial_connection(void)
{
    // allocate serial handle
    if((ser_ = serial_new()) == NULL)
    {
        // TODO: Bad alloc
        return -1;
    }
    // setup serial connection
    if(serial_open(ser_, SERIAL_PORT, SERIAL_BAUD) < 0)
    {
        // TODO: Better error log
        fprintf(stderr, "serial_open(): %s\n", serial_errmsg(ser_));
        FREE_SERIAL_HANDLE(ser_);
        return -1;
    }
    // clear serial port buffer
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
    if(pthread_create(&tinfo_, NULL, proto_comms_serial_thread_func, NULL) != 0)
    {
        fprintf(stderr, "Error creating pthread\n");
        FREE_SERIAL_HANDLE(ser_);
        return -1;
    }

    return 0;
}

void proto_comms_serial_thread_cleanup(void *arg)
{
    (void)arg;
    threadRunning_ = false;

    Message stop = Message_init_zero;
    stop.type = CmdType_STOP;
    stop.which_payload = Message_stopDefault_tag;
    stop.payload.stopDefault = true;

    // fire and forget
    proto_comms_send_msg(&stop);

    serial_close(ser_);
    FREE_SERIAL_HANDLE(ser_);
}

void proto_comms_stop_serial_connection(void)
{
    pthread_cancel(tinfo_);
    pthread_join(tinfo_, NULL);
}

static void *proto_comms_serial_thread_func(void *arg)
{
    (void)arg;

    // NOTE: Name can only be 16 bytes long including null terminator
    prctl(PR_SET_NAME, "UltraSerial", 0, 0, 0);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    pthread_cleanup_push(proto_comms_serial_thread_cleanup, NULL);

    // add setup command as first cmd buffer
    Message msg = Message_init_zero;
    msg.type = CmdType_HELLO_THERE;
    message_handler_add_outgoing(&msg);

    threadRunning_ = true;
    while(threadRunning_)
    {
        if(serial_poll(ser_, 0))
        {
            Response resp = Response_init_zero;
            if(proto_comms_read_response(&resp) < 0)
            {
                // TODO: Error log
                printf("Error reading response\n");
                break;
            }

            if(message_handler_add_incoming(&resp) < 0)
            {
                printf("Error appending incoming response\n");
                break;
            }
        }

        if(message_handler_process() < 0)
        {
            break;
        }
    }

    pthread_cleanup_pop(0);

    return NULL;
}
