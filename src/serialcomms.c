#include "serialcomms.h"
#include "callbacks_position.h"
#include "serial.h"

#include <stdio.h>
#include <stdint.h>

typedef struct PositionCountPair
{
    ComboPositions_t pos;
    uint16_t count;
} PositionCountPair_t;

static inline int recipe_convert_positions(const Recipe *rec, PositionCountPair_t *pc)
{
    for(uint16_t i = 0; i < rec->ingCount; i++)
    {
        const ComboPositions_t pos = cb_get_position_by_id(rec->ingPairs[i].id);
        if(pos == PAGES_COMBO_POS_INVALID)
            return -1;

        pc[i].pos = pos;
        pc[i].count = rec->ingPairs[i].count;
    }

    return 0;
}

static int _comms_start_new_mixing_impl(PositionCountPair_t *pc, uint16_t count)
{
    // TODO: Add position movements to a queue and return
    for(int i = 0; i < count; ++i)
        printf("start new mixing impl %d\n", pc[i].pos);
    return 0;
}

int comms_start_new_mixing(Recipe *rec)
{
    if(!rec)
        return -1;

    PositionCountPair_t pc[rec->ingCount];
    memset(&pc, 0, sizeof(*pc) * rec->ingCount);

    if(recipe_convert_positions(rec, pc) < 0)
        return -1;

    return _comms_start_new_mixing_impl(pc, rec->ingCount);
}


static int _comms_start(serial_t *serial)
{
    uint8_t s[] = "Hello World!";
    uint8_t buf[128];
    int ret;

    /* Open /dev/ttyUSB0 with baudrate 115200, and defaults of 8N1, no flow control */
    if (serial_open(serial, "./ttyV0", 115200) < 0) {
        fprintf(stderr, "serial_open(): %s\n", serial_errmsg(serial));
        return 1;
    }

    /* Write to the serial port */
    if (serial_write(serial, s, sizeof(s)) < 0) {
        fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
        return 1;
    }

    /* Read up to buf size or 2000ms timeout */
    if ((ret = serial_read(serial, buf, sizeof(buf), 2000)) < 0) {
        fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
        return 1;
    }

    printf("read %d bytes: _%s_\n", ret, buf);

    return 0;
}

int comms_start_serial_connection(void)
{
    serial_t *serial = serial_new();
    const int ret = _comms_start(serial);
    serial_close(serial);

    return ret;
}

