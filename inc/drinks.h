#ifndef __DRINKS_H__
#define __DRINKS_H__

#include <stdbool.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include "drinkarray.h"

/* functions */

struct DrinkManagement
{
    Ing_Array_t *ingredientArray;
    Rec_Array_t *recipeArray;

    GtkListStore *ingredientListStore;
    GtkListStore *recipeListStore;
};

Ing_Array_t *get_all_ingredients();
Rec_Array_t *get_all_recipes();

#endif //__DRINKS_H__
