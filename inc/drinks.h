#ifndef __DRINKS_H__
#define __DRINKS_H__

#include <stdbool.h>
#include <stdint.h>
#include "helper.h"
#include "drinkarray.h"
#include <gtk/gtk.h>

/* functions */

Ing_Array_t *getAllIngredients();
Rec_Array_t *getAllRecipes();

#endif //__DRINKS_H__