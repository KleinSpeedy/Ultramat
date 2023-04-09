#ifndef __DRINKARRAY_H__
#define __DRINKARRAY_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* types */

typedef struct ingredient
{
    char *name;
    uint8_t position;
    uint8_t id;
    bool selected;
} Ingredient_t;

typedef struct drinktuple
{
    uint8_t id;
    uint8_t quantity;
} DrinkTuple_t;

typedef struct recipe
{
    char *name;

    DrinkTuple_t *recipeTuples;
    uint8_t ingredientCount;
    
    uint8_t id;
    bool available;
} Recipe_t;

typedef struct ingredients_array
{
    size_t size;
    Ingredient_t *ingredients;
} Ing_Array_t;

typedef struct recipe_array
{
    size_t size;
    Recipe_t *recipes;
} Rec_Array_t;

/* functions */

Ing_Array_t *ingredients_array_create(size_t size);
void ingredients_array_delete(Ing_Array_t *array);
Ingredient_t ingredients_get_at(Ing_Array_t *array, size_t pos);
void ingredients_set_at(Ing_Array_t *array, size_t pos, Ingredient_t ing);

Rec_Array_t *recipe_array_create(size_t size);
void recipe_array_delete(Rec_Array_t *array);
Recipe_t recipe_get_at(Rec_Array_t *array, size_t pos);
void recipe_set_at(Rec_Array_t *array, size_t pos, Recipe_t rec);

#endif //__DRINKARRAY_H__