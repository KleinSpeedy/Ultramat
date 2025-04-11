#include "message_handler.h"
#include "callbacks_position.h"
#include "command_handler.h"
#include "serialcomms.h"
#include "util/cmd_buffer.h"
#include "util/queue.h"
#include "util/ring_buffer.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

typedef enum worker_state
{
    WORKER_STATE_IDLE = 0,
    WORKER_STATE_SEND_CMD,
    WORKER_STATE_ACK_PENDING,
    WORKER_STATE_DONE_PENDING,
    WORKER_STATE_ERROR
} eWorkerState;

typedef enum error_type
{
    ERROR_UNKNOWN_MSG,      // invalid cmd byte in response msg
    ERROR_OOO_ACK,          // recieved acknowledge arrived out of order
    ERROR_OOO_DONE,         // recieved done arrived out of order
    ERROR_OOO_MSG,          // recieved msg arrived out of order
    ERROR_ACK_TIMEOUT,      // acknowledge msg timeout reached
    ERROR_DONE_TIMEOUT,     // done msg timeout was reached
    ERROR_SERIAL_WRITE,     // error during serial write ??
    ERROR_SERIAL_READ,      // error during serial read ??
    ERROR_RECIEVED_ERR,     // microcontroller sent an error

    ERROR_NUM
} eErrorType;

#define WORKER_TIMEOUT_ACK      10.0f // seconds
#define WORKER_TIMEOUT_DONE     60.0f // seconds

typedef struct message_handler
{
    ring_buffer_t pendingAcks;
    ring_buffer_t pendingDones;

    eWorkerState fsmState;
    eErrorType error;

    bool timeoutAckStarted;
    time_t timestampAck;

    bool timeoutDoneStarted;
    time_t timestampDone;
} message_handler_t;

static message_handler_t worker_;

// command queues for incoming and outgoing messages
static command_queue_t incomingMsg_;
static command_queue_t outgoingMsg_;

static const char *message_handler_error_string(const eErrorType err)
{
    assert(err >= 0 && err < ERROR_NUM);
    static const char* errorStrings[ERROR_NUM] = {
        "Unknown message recieved",
        "Out of order acknowledge message recieved",
        "Out of order done message recieved",
        "Incorrect order for recieved message",
        "Acknowledge message timeout",
        "Done message timeout",
        "Serial write failed",
        "Serial read failed",
        "Error message recieved"
    };

    return errorStrings[err];
}

static cmd_msg_t cmd_msg_from_cmd_buffer(const uint8_t id,
                                        const cmd_buffer_t *cmdBuffer)
{
    uint16_t checksum = FRAME_VAL_START + id + cmdBuffer->cmd + cmdBuffer->size;
    for(uint8_t i = 0; i < cmdBuffer->size; ++i)
        checksum += cmdBuffer->data[i];

    return (cmd_msg_t){id, *cmdBuffer, checksum};
}

// returns an ID between 0 and 255 and increments the id counter
static uint8_t get_next_out_msg_id(void)
{
    static uint8_t idCounter_ = 0;
    const uint8_t nextId = idCounter_;
    // id is sent as one byte, so we need to wrap around
    idCounter_ = (idCounter_ + 1) % 255;
    return nextId;
}

static int worker_add_new_pendings(const uint8_t id)
{
    if (ring_buffer_is_full(&worker_.pendingAcks) ||
        ring_buffer_is_full(&worker_.pendingDones))
    {
        // TODO: do something in case of full!
        printf("Error: Ring buffer is full\n");
        return -1;
    }

    return ring_buffer_put(&worker_.pendingAcks, id) | 
            ring_buffer_put(&worker_.pendingDones, id);
}

static inline void handler_set_error(const eErrorType type)
{
    worker_.fsmState = WORKER_STATE_ERROR;
    worker_.error = type;
}
static eWorkerState handler_process_incoming(const cmd_msg_t *msg)
{
    eWorkerState nextState = worker_.fsmState;

    switch(msg->cmdBuffer.cmd)
    {
        case RESPONSE_ACK:
        {
            if(worker_.fsmState != WORKER_STATE_ACK_PENDING)
            {
                handler_set_error(ERROR_OOO_MSG);
                nextState = WORKER_STATE_ERROR;
                break;
            }
            // Check if its the acknowledge we are waiting for
            const uint8_t incomingAckId = msg->cmdBuffer.data[0];
            uint8_t pendingAckId = 0;
            ring_buffer_peek(&worker_.pendingAcks, &pendingAckId);

            if(pendingAckId == incomingAckId)
            {
                nextState = WORKER_STATE_DONE_PENDING;
                // advance ring buffer, throw away result
                ring_buffer_pop(&worker_.pendingAcks, &pendingAckId);
            }
            else
            {
                handler_set_error(ERROR_OOO_ACK);
                nextState = WORKER_STATE_ERROR;
            }
            break;
        }
        case RESPONSE_DONE:
        {
            if(worker_.fsmState != WORKER_STATE_DONE_PENDING)
            {
                handler_set_error(ERROR_OOO_MSG);
                nextState = WORKER_STATE_ERROR;
                break;
            }

            // Check if its the done we are waiting for
            const uint8_t incomingDoneId = msg->cmdBuffer.data[0];
            uint8_t pendingDoneId = 0;
            ring_buffer_peek(&worker_.pendingDones, &pendingDoneId);

            if(pendingDoneId == incomingDoneId)
            {
                // Execute corresponding command handler with payload and size
                command_handler_execute_done(pendingDoneId,
                        msg->cmdBuffer.data,
                        msg->cmdBuffer.size);
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
        case RESPONSE_ERROR:
        {
            handler_set_error(ERROR_RECIEVED_ERR);
            nextState = WORKER_STATE_ERROR;
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
    int ret = cmd_queue_init(&incomingMsg_);
    ret += cmd_queue_init(&outgoingMsg_);
    if(ret > 0)
        return -1;
    // zero out worker struct, including queues
    memset(&worker_, 0, sizeof(message_handler_t));

    return 0;
}

int message_handler_deinit(void)
{
    // TODO: Wait for signal that its ok to clear
    cmd_queue_free(&incomingMsg_);
    cmd_queue_free(&outgoingMsg_);
    return 0;
}

int message_handler_add_incoming(const cmd_msg_t msg)
{
    if(cmd_queue_push(&incomingMsg_, &msg) < 0)
    {
        // TODO: Error log
        return -1;
    }

    return 0;
}

// TODO: 2 threads potentially access this function, avoid race conditions!!
int message_handler_add_outgoing(const cmd_buffer_t cmdBuffer)
{
    const uint8_t nextId = get_next_out_msg_id();
    const cmd_msg_t msg = cmd_msg_from_cmd_buffer(nextId, &cmdBuffer);

    if(worker_add_new_pendings(nextId) < 0)
    {
        // TODO: Error log
        return -1;
    }

    if(cmd_queue_push(&outgoingMsg_, &msg) < 0)
    {
        // TODO: Error log
        return -1;
    }

    command_handler_add(msg.id, msg.cmdBuffer.cmd);
    return 0;
}

void message_handler_process(void)
{
    time_t now = time(NULL);

    // TODO: What about out-of-order arrival??

    cmd_msg_t in = {0}, out = {0};
    switch(worker_.fsmState)
    {
        case WORKER_STATE_IDLE:
        {
            // read even if we dont expect a message to arrive in case of error
            if(cmd_queue_pop(&incomingMsg_, &in) == 0)
            {
                worker_.fsmState = handler_process_incoming(&in);
            }
            else if(!cmd_queue_is_empty(&outgoingMsg_))
            {
                worker_.fsmState = WORKER_STATE_SEND_CMD;
            }
            break;
        }
        case WORKER_STATE_SEND_CMD:
        {
            if(cmd_queue_pop(&outgoingMsg_, &out) < 0)
            {
                worker_.fsmState = WORKER_STATE_ERROR;
                break;
            }
            if(comms_send_msg(&out) < 0)
            {
                // TODO: Error State?
                break;
            }
            worker_.fsmState = WORKER_STATE_ACK_PENDING;
            break;
        }
        case WORKER_STATE_ACK_PENDING:
        {
            if(cmd_queue_pop(&incomingMsg_, &in) == 0)
            {
                worker_.timeoutAckStarted = false;
                worker_.fsmState = handler_process_incoming(&in);
            }
            else
            {
                // start timeout
                // We wait 10s for ack msg, after that we enter error
                if(!worker_.timeoutAckStarted)
                {
                    worker_.timeoutAckStarted = true;
                    worker_.timestampAck = now;
                    break;
                }
                else if(difftime(now, worker_.timestampAck) > WORKER_TIMEOUT_ACK)
                {
                    handler_set_error(ERROR_ACK_TIMEOUT);
                }
            }
            break;
        }
        case WORKER_STATE_DONE_PENDING:
        {
            if(cmd_queue_pop(&incomingMsg_, &in) == 0)
            {
                worker_.timeoutDoneStarted = false;
                worker_.fsmState = handler_process_incoming(&in);
            }
            else
            {
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
            if(!cmd_queue_is_empty(&outgoingMsg_))
                worker_.fsmState = WORKER_STATE_SEND_CMD;
            break;
        }
        case WORKER_STATE_ERROR:
        {
            cb_error_serial_communication(
                    message_handler_error_string(worker_.error));
            break;
        }
    }
}
