#ifndef DRINKLISTS_H
#define DRINKLISTS_H

#include "util/dynamic_array.h"

// Column values for ingredients
typedef enum {
    ING_COLUMN_NAME,
    ING_COLUMN_ID,

    ING_NUM_COLUMN
}Ingredient_Columns;

// Column values for recipes
typedef enum {
    REC_COLUMN_NAME,
    REC_COLUMN_ID,

    REC_NUM_COLUMNS
}Recipe_Columns;

// Create new Ingredients list store
void lists_create_ingredient_store(VLArray_t *ingArray);
// Create new Recipe list store
void lists_create_recipe_store(VLArray_t *recArray);

// Get pointer to ingredient list store
void *lists_ingredient_store(void);
// Get pointer to recipe list store
void *lists_recipe_store(void);

#endif // DRINKLISTS_H
