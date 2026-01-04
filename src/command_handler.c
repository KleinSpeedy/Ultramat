#include "command_handler.h"
#include "callbacks_position.h"
#include "gdk/gdk.h"
#include "proto/commands.pb.h"

#include <stdio.h>
#include <string.h>

#define COMMANDS_MAX_PENDING 32

typedef int (*handler_func)(const Response *);

typedef struct cmd_func_pair
{
    CmdType cmd;
    handler_func func;
} cmd_func_pair_t;

typedef struct id_handler_pair
{
    uint8_t id;
    handler_func func;
} id_handler_pair_t;

/* BEGIN command done handler function declaration */

static int hello_there_handler(const Response *);
static int recipe_step_handler(const Response *);
static int move_x_handler(const Response *);
static int move_y_handler(const Response *);

/* END command handler function declaration */

static const cmd_func_pair_t cmdFuncMap_[_CmdType_ARRAYSIZE] = {
    {CmdType_CMD_UNDEFINED, NULL}, // no handler for default command
    {CmdType_HELLO_THERE, hello_there_handler},
    {CmdType_STOP, NULL}, // no handler for stop command
    {CmdType_RECIPE_STEP, recipe_step_handler},
    {CmdType_MOVE_X, move_x_handler},
    {CmdType_MOVE_Y, move_y_handler},
};

static id_handler_pair_t pendingCmds_[COMMANDS_MAX_PENDING];
static uint8_t handlerIndex_;

static handler_func get_handler_func(const CmdType cmd)
{
    for(uint8_t i = 0; i < _CmdType_ARRAYSIZE; ++i)
    {
        if(cmdFuncMap_[i].cmd == cmd)
        {
            return cmdFuncMap_[i].func;
        }
    }
    return NULL;
}

static int execute_cmd_handler(const Response *resp)
{
    for(uint8_t i = 0; i < COMMANDS_MAX_PENDING; ++i)
    {
        if(pendingCmds_[i].id == resp->id)
        {
            // return 0 if no handler defined
            return pendingCmds_[i].func != NULL ? pendingCmds_[i].func(resp)
                                                : 0;
        }
    }
    // no handler found
    return -1;
}

static int hello_there_handler(const Response *resp)
{
    if(!resp)
        return -1;

    gdk_threads_add_idle(cb_cmd_hello_there_done, (gpointer)resp);
    return 0;
}

static int recipe_step_handler(const Response *resp)
{
    (void)resp;

    gdk_threads_add_idle(cb_cmd_step_done, NULL);
    return 0;
}

static int move_x_handler(const Response *resp)
{
    (void)resp;

    gdk_threads_add_idle(cb_cmd_move_x_done, NULL);
    return 0;
}

static int move_y_handler(const Response *resp)
{
    (void)resp;

    gdk_threads_add_idle(cb_cmd_move_y_done, NULL);
    return 0;
}

/* public functions */

void command_handler_init(void)
{
    handlerIndex_ = 0;
    // zero out array
    memset(pendingCmds_, 0, sizeof(id_handler_pair_t) * COMMANDS_MAX_PENDING);
}

int command_handler_add(const uint8_t id, const CmdType cmd)
{
    const handler_func handler = get_handler_func(cmd);
    if(!handler)
        return -1;

    pendingCmds_[handlerIndex_].id = id;
    pendingCmds_[handlerIndex_].func = handler;
    // just wrap around
    handlerIndex_ = (handlerIndex_ + 1) % COMMANDS_MAX_PENDING;
    return 0;
}

int command_handler_execute_done(const Response *resp)
{
    return execute_cmd_handler(resp);
}
