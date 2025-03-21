#ifndef CMD_BUFFER_H
#define CMD_BUFFER_H

#include <stdint.h>

// manually increment this when adding a new command
#define COMMANDS_COUNT  7
// Messages from Base to Arduino
typedef enum eCommand
{
    CMD_HELLO_THERE = 0xB0,
    CMD_STOP = 0xB1,

    CMD_HOMING_X = 0xC0,
    CMD_HOMING_Y,
    CMD_MOVE,
    CMD_MOVE_X,
    CMD_MOVE_Y
} eCommand;

// Messages from Arduino to Base
typedef enum eResponse
{
    RESPONSE_ACK = 0xA0,    // Arduino acknowledges message from base
    RESPONSE_DONE = 0xA1,   // signals command is finished
    RESPONSE_ERROR = 0xAE   // some error occured on Arduino
} eResponse;

// TODO: Does this belong here?
typedef enum eMsgFramePosition
{
    FRAME_POS_INVALID = -1,
    FRAME_POS_START = 0,
    FRAME_POS_ID,
    FRAME_POS_CMD,
    FRAME_POS_DATA_LENGTH,
    FRAME_POS_DATA
} eMsgFramePosition;

// Byte values for specific bytes in message frame
typedef enum eMsgFrameValues
{
    FRAME_VAL_START = 0x10,
    FRAME_VAL_XOR = 0x20,
    FRAME_VAL_ESCAPE = 0x7D
} eMsgFrameValues;

// returns the real byte size for a command including:
// id + cmd + size + checksum upper + checksum lower = 6 bytes + payload size
#define CMD_BUFFER_TO_BYTE_SIZE(x)     (x.size + 6)
#define CMD_BUFFER_SIZE 128
typedef struct command_buffer
{
    uint8_t cmd;
    uint8_t size;
    uint8_t data[CMD_BUFFER_SIZE];
} cmd_buffer_t;

// print a command buffer struct
void cmd_buffer_print(const cmd_buffer_t *cmd_buf);

// Setup command always has the same layout so we can define it here
#define CMD_BUFFER_HELLO_THERE (cmd_buffer_t){ CMD_HELLO_THERE, 1, {0x00} }
#define CMD_BUFFER_MOVE(pos, count) (cmd_buffer_t){ CMD_MOVE, 2, {pos, count} }

#define CMD_BUFFER_HOMING_X (cmd_buffer_t){ CMD_HOMING_X, 1, {0x00} }
#define CMD_BUFFER_HOMING_Y (cmd_buffer_t){ CMD_HOMING_Y, 1, {0x00} }

#endif // CMD_BUFFER_H
