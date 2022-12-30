#ifndef __DRINKLISTS_H__
#define __DRINKLISTS_H__

#include <gtk/gtk.h>
#include "drinkarray.h"

/* Main control for ingredients and recipes 
 * uses GTK Liststore for selection of drinks */

GtkListStore *createIngredientListStore(Ing_Array_t *ingArray);

#endif //__DRINKLISTS_H__