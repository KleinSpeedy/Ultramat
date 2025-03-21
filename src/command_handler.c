#include "command_handler.h"
#include "callbacks_position.h"
#include "util/cmd_buffer.h"
#include "util/queue.h"


#include <stdio.h>
#include <string.h>


#define COMMANDS_MAX_PENDING    QUEUE_MAX_SIZE

typedef int (*handler_func)(void);

typedef struct cmd_func_pair
{
    eCommand cmd;
    handler_func func;
} cmd_func_pair_t;

typedef struct id_handler_pair
{
    uint8_t id;
    handler_func func;
} id_handler_pair_t;

/* BEGIN command done handler function declaration */

static int hello_there_handler(void);
static int homing_x_handler(void) {return 0;}
static int homing_y_handler(void) {return 0;}
static int move_handler(void);
static int move_x_handler(void) {return 0;}
static int move_y_handler(void) {return 0;}

/* END command handler function declaration */

static const cmd_func_pair_t cmdFuncMap_[COMMANDS_COUNT] =
{
    {CMD_HELLO_THERE, hello_there_handler},
    {CMD_STOP, NULL},   // no handler for stop command
    {CMD_HOMING_X, homing_x_handler},
    {CMD_HOMING_Y, homing_y_handler},
    {CMD_MOVE, move_handler},
    {CMD_MOVE_X, move_x_handler},
    {CMD_MOVE_Y, move_y_handler},
};

static id_handler_pair_t pendingCmds_[COMMANDS_MAX_PENDING];
static uint8_t handlerIndex_;

static handler_func get_handler_func(const eCommand cmd)
{
    for(uint8_t i = 0; i < COMMANDS_COUNT; ++i)
    {
        if(cmdFuncMap_[i].cmd == cmd)
        {
            return cmdFuncMap_[i].func;
        }
    }
    return NULL;
}

static int execute_cmd_handler(const uint8_t id)
{
    for(uint8_t i = 0; i < COMMANDS_MAX_PENDING; ++i)
    {
        if(pendingCmds_[i].id == id)
        {
            // always return 0 even if no handler defined
            return pendingCmds_[i].func != NULL ?
                pendingCmds_[i].func() : 0;
        }
    }
    // no handler found
    return -1;
}

static int hello_there_handler(void)
{
    cb_cmd_hello_there_done();
    return 0;
}

static int move_handler(void)
{
    cb_cmd_move_done();
    return 0;
}

/* public functions */

void command_handler_init(void)
{
    handlerIndex_ = 0;
    // zero out array
    memset(pendingCmds_, 0, sizeof(id_handler_pair_t) * COMMANDS_MAX_PENDING);
}

int command_handler_add(const uint8_t id, const eCommand cmd)
{
    const handler_func handler = get_handler_func(cmd);
    if(!handler)
        return -1;

    pendingCmds_[handlerIndex_].id = id;
    pendingCmds_[handlerIndex_].func = handler;
    handlerIndex_++;
    return 0;
}

int command_handler_execute_done(const uint8_t id)
{
    return execute_cmd_handler(id);
}
