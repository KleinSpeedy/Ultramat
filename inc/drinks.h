#ifndef __DRINKS_H__
#define __DRINKS_H__

#include <stdbool.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include "drinkarray.h"

/* functions */

typedef struct drinkManagement
{
    Ing_Array_t *ingredientArray;
    Rec_Array_t *recipeArray;

    // save last ingredient to track changes
    Ingredient_t *last;

    GtkListStore *ingredientListStore;
    GtkListStore *recipeListStore;
} DrinkManagement_t;

Ing_Array_t *get_all_ingredients();
Rec_Array_t *get_all_recipes();

#endif //__DRINKS_H__