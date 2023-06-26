#ifndef __DRINKLISTS_H__
#define __DRINKLISTS_H__

#include <gtk/gtk.h>
#include "recipe.h"

/* Main control for ingredients and recipes 
 * uses GTK Liststore for selection of drinks */

// Column values for ingredients
typedef enum {
    ING_COLUMN_NAME,
    ING_COLUMN_OBJECT,

    ING_NUM_COLUMN
}Ingredient_Columns;

// Column values for recipes
typedef enum {
    REC_COLUMN_NAME,
    REC_COLUMN_OBJECT,

    REC_NUM_COLUMNS
}Recipe_Columns;

/* TODO: Where to put these functions? Duplicate implementation in recipe/ingredient.c ?
gboolean
lists_ingredient_is_selected();
gboolean
lists_recipe_is_available();
*/

// Create new Ingredients list store
void
lists_create_ingredient_store();
// Create new Recipe list store
void
lists_create_recipe_store();

// Get pointer to ingredient list store
GtkListStore *
lists_ingredient_store();
// Get pointer to recipe list store
GtkListStore *
lists_recipe_store();

void
lists_ingredient_append(UIngredient *ing);
UIngredient *
lists_ingredient_get_by_id(guint ingID);
UIngredient *
lists_ingredient_get_by_position(guint8 position);

GtkTreeIter *
lists_iter_get_by_position(gint8 position);

void
lists_recipe_append(URecipe *rec);
URecipe *
lists_recipe_get_active_recipe();

#ifdef ULTRA_DEBUG
void
dbg_print_ingredients();
void
dbg_print_recipes();
#endif // ULTRA_DEBUG

#endif //__DRINKLISTS_H__
