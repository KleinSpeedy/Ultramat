#ifndef __DRINKLISTS_H__
#define __DRINKLISTS_H__

#include <gtk/gtk.h>
#include "drinkarray.h"

/* Main control for ingredients and recipes 
 * uses GTK Liststore for selection of drinks */

// Column values for ingredients
static enum {
    ING_COLUMN_NAME,
    ING_COLUMN_ID,
    ING_COLUMN_POSITION,
    ING_COLUMN_SELECTED,

    ING_NUM_COLUMN
}Ingredient_Columns;

// Column values for recipes
static enum {
    REC_COLUMN_NAME,
    REC_COLUMN_ID,
    REC_COLUMN_AVAILABLE,

    REC_NUM_COLUMNS
}Recipe_Columns;

GtkListStore *create_ingredient_listStore(Ing_Array_t *ingArray);

GtkListStore *create_recipe_listStore(Rec_Array_t *recArray);

#endif //__DRINKLISTS_H__
