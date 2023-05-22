#ifndef __DRINKS_H__
#define __DRINKS_H__

#include <stdbool.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include "drinkarray.h"
#include "recipe.h"

/* functions */

typedef enum
{
    DRINKS_OK = 0,  /* No errors during reading files */
    DRINKS_ERROR,   /* Error during reading files */

    DRINKS_STATUS_NUM
} InputStatus_t;

Ing_Array_t *get_all_ingredients();
Rec_Array_t *get_all_recipes();

guint
drinks_io_get_num_ingredients();
guint
drinks_io_get_num_recipes();

InputStatus_t
drinks_io_read_ingredients();

InputStatus_t
drinks_io_read_recipes();

#endif //__DRINKS_H__
