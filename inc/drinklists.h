#ifndef __DRINKLISTS_H__
#define __DRINKLISTS_H__

#include <gtk/gtk.h>
#include "drinkarray.h"

/* Main control for ingredients and recipes 
 * uses GTK Liststore for selection of drinks */

GtkListStore *create_ingredient_listStore(Ing_Array_t *ingArray);

GtkListStore *create_recipe_listStore(Rec_Array_t *recArray);

#endif //__DRINKLISTS_H__