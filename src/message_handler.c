#include "message_handler.h"
#include "callbacks_position.h"
#include "command_handler.h"
#include "proto/commands.pb.h"
#include "proto_serial.h"
#include "util/queue.h"
#include "util/ring_buffer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef enum worker_state
{
    WORKER_STATE_IDLE = 0,
    WORKER_STATE_SEND_CMD,
    WORKER_STATE_DONE_PENDING,
    WORKER_STATE_ERROR
} eWorkerState;

typedef enum error_type
{
    ERROR_UNKNOWN_MSG,  // invalid cmd byte in response msg
    ERROR_OOO_DONE,     // recieved done arrived out of order
    ERROR_OOO_MSG,      // recieved msg arrived out of order
    ERROR_DONE_TIMEOUT, // done msg timeout was reached
    ERROR_SERIAL_WRITE, // error during serial write ??
    ERROR_SERIAL_READ,  // error during serial read ??
    ERROR_RECIEVED_ERR, // microcontroller sent an error

    ERROR_NUM
} eErrorType;

#define WORKER_TIMEOUT_DONE 90.0f // seconds

typedef struct message_handler
{
    ring_buffer_t pendingDones;

    eWorkerState fsmState;
    eErrorType error;

    bool timeoutDoneStarted;
    time_t timestampDone;
} message_handler_t;

static message_handler_t worker_;

// command queues for incoming responses and outgoing messages
static in_resp_q inQ_;
static out_msg_q outQ_;

static const char *message_handler_error_string(const eErrorType err)
{
    assert(err >= 0 && err < ERROR_NUM);
    static const char *errorStrings[ERROR_NUM] = {
        "Unknown message recieved",
        "Out of order done message recieved",
        "Incorrect order for recieved message",
        "Done message timeout",
        "Serial write failed",
        "Serial read failed",
        "Error message recieved"};

    return errorStrings[err];
}

// returns an ID and increments the id counter
static uint32_t get_next_out_msg_id(void)
{
    static uint32_t idCounter_ = 0;
    idCounter_ += 1;
    return idCounter_;
}

static int worker_add_new_pendings(const uint8_t id)
{
    if(ring_buffer_is_full(&worker_.pendingDones))
    {
        // TODO: do something in case of full!
        printf("Error: Ring buffer is full\n");
        return -1;
    }

    return ring_buffer_put(&worker_.pendingDones, id);
}

static inline void handler_set_error(const eErrorType type)
{
    worker_.fsmState = WORKER_STATE_ERROR;
    worker_.error = type;
}

static eWorkerState handler_process_incoming(const Response *resp)
{
    eWorkerState nextState = worker_.fsmState;

    switch(resp->type)
    {
    case ResponseType_DONE:
    {
        if(worker_.fsmState != WORKER_STATE_DONE_PENDING)
        {
            handler_set_error(ERROR_OOO_MSG);
            nextState = WORKER_STATE_ERROR;
            break;
        }

        // Check if its the done we are waiting for
        const uint8_t incomingDoneId = resp->id;
        uint8_t pendingDoneId = 0;
        ring_buffer_peek(&worker_.pendingDones, &pendingDoneId);

        if(pendingDoneId == incomingDoneId)
        {
            // Execute corresponding command handler with payload and size
            command_handler_execute_done(resp);

            // advance ring buffer, throw away result
            ring_buffer_pop(&worker_.pendingDones, &pendingDoneId);

            if(ring_buffer_is_empty(&worker_.pendingDones))
                nextState = WORKER_STATE_IDLE;
            else
                nextState = WORKER_STATE_DONE_PENDING;
        }
        else
        {
            handler_set_error(ERROR_OOO_DONE);
            nextState = WORKER_STATE_ERROR;
        }
        break;
    }
    case ResponseType_ERROR:
    {
        handler_set_error(ERROR_RECIEVED_ERR);
        nextState = WORKER_STATE_ERROR;
        break;
    }
    case ResponseType_EMERGENCY_STOP:
    {
        break;
    }
    default:
    {
        handler_set_error(ERROR_UNKNOWN_MSG);
        nextState = WORKER_STATE_ERROR;
    }
    }

    return nextState;
}

/* public functions */

int message_handler_init(void)
{
    // init command handler here as message handler is only one with access
    command_handler_init();
    int ret = in_resp_q_init(&inQ_);
    ret += out_msg_q_init(&outQ_);
    if(ret > 0)
        return -1;
    // zero out worker struct, including queue
    memset(&worker_, 0, sizeof(message_handler_t));

    return 0;
}

int message_handler_add_incoming(const Response *resp)
{
    if(in_resp_q_push(&inQ_, resp) < 0)
    {
        // TODO: Error log
        return -1;
    }

    return 0;
}

// TODO: 2 threads potentially access this function, avoid race conditions!!
int message_handler_add_outgoing(Message *const msg)
{
    const uint8_t nextId = get_next_out_msg_id();
    msg->id = nextId;

    if(worker_add_new_pendings(nextId) < 0)
    {
        // TODO: Error log
        return -1;
    }

    if(out_msg_q_push(&outQ_, msg) < 0)
    {
        // TODO: Error log
        return -1;
    }

    printf("Adding id %d type %d\n", msg->id, msg->type);
    command_handler_add(nextId, msg->type);
    return 0;
}

int message_handler_move_x(const PositionX pos)
{
    if(pos <= PositionX_INVALID || pos > PositionX_FINAL)
        return -1;

    Message out = Message_init_zero;
    out.type = CmdType_MOVE_X;
    out.which_payload = Message_xPos_tag;
    out.payload.xPos = pos;

    return message_handler_add_outgoing(&out);
}

int message_handler_move_y(const uint8_t count)
{
    // TODO: Magic number, set this somewhere
    if(count <= 0 || count > 10)
        return -1;

    Message out = Message_init_zero;
    out.type = CmdType_MOVE_Y;
    out.which_payload = Message_yCount_tag;
    out.payload.yCount = count;

    return message_handler_add_outgoing(&out);
}

int message_handler_process(void)
{
    Response resp = Response_init_zero;
    Message msg = Message_init_zero;

    int ret = 0;

    switch(worker_.fsmState)
    {
    case WORKER_STATE_IDLE:
    {
        // read even if we dont expect a message to arrive in case of error
        if(in_resp_q_pop(&inQ_, &resp) == 0)
        {
            worker_.fsmState = handler_process_incoming(&resp);
        }
        else if(!out_msg_q_is_empty(&outQ_))
        {
            worker_.fsmState = WORKER_STATE_SEND_CMD;
        }
        break;
    }
    case WORKER_STATE_SEND_CMD:
    {
        if(out_msg_q_pop(&outQ_, &msg) < 0)
        {
            worker_.fsmState = WORKER_STATE_ERROR;
            break;
        }
        if(proto_comms_send_msg(&msg) < 0)
        {
            // TODO: Error State?
            worker_.fsmState = WORKER_STATE_ERROR;
            break;
        }
        worker_.fsmState = WORKER_STATE_DONE_PENDING;
        break;
    }
    case WORKER_STATE_DONE_PENDING:
    {
        if(in_resp_q_pop(&inQ_, &resp) == 0)
        {
            worker_.timeoutDoneStarted = false;
            worker_.fsmState = handler_process_incoming(&resp);
        }
        else
        {
            time_t now = time(NULL);

            // start timeout
            // We wait 60s for done msg, after that we enter error
            if(!worker_.timeoutDoneStarted)
            {
                worker_.timeoutDoneStarted = true;
                worker_.timestampDone = now;
            }
            else if(difftime(now, worker_.timestampDone) > WORKER_TIMEOUT_DONE)
            {
                handler_set_error(ERROR_DONE_TIMEOUT);
                break;
            }
        }
        // while waiting for done, we allow to send new messages
        if(!out_msg_q_is_empty(&outQ_))
            worker_.fsmState = WORKER_STATE_SEND_CMD;
        break;
    }
    case WORKER_STATE_ERROR:
    {
        cb_error_serial_communication(
            message_handler_error_string(worker_.error));
        ret = -1;
        break;
    }
    }

    return ret;
}
