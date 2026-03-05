#ifndef DRINKS_DB_H
#define DRINKS_DB_H

#include <gtk/gtk.h>

// List store values
// Column values for ingredients
enum
{
    ING_COLUMN_NAME,
    ING_COLUMN_ID,
    ING_COLUMN_ICON,

    ING_NUM_COLUMN
};

// Column values for recipes
enum
{
    REC_COLUMN_NAME,
    REC_COLUMN_ID,

    REC_NUM_COLUMNS
};

typedef struct recipe_info
{
    int type;
    int position;
    int count;
    int unit;
} recipe_info_t;

int drinks_check_recipe(const int recipe_id);

int drinks_set_ing_pos(const int ing_id, const int pos);
void drinks_reset_ing_pos(const int pos);

int drinks_ing_liquor_list_store(GtkListStore *ls);
int drinks_ing_pump_list_store(GtkListStore *ls);

int drinks_rec_list_store(GtkListStore *ls);

#endif // DRINKS_DB_H
