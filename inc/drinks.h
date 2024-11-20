#ifndef DRINKS_H
#define DRINKS_H

#include "dynamic_array.h"

#include <stdint.h>

#define MAX_NAME_LENGTH     128

typedef struct Ingredient
{
    char name[MAX_NAME_LENGTH];
    uint16_t id;
} Ingredient;

typedef struct IdCountPair
{
    uint16_t id;
    uint16_t count;
} IdCountPair;

typedef struct Recipe
{
    char name[MAX_NAME_LENGTH];
    uint16_t id;

    IdCountPair *ingPairs;
    uint16_t ingCount;
} Recipe;

/**
 * @brief Get number of ingredients read in from file
 */
uint32_t drinks_io_get_num_ingredients(void);

/**
 * @brief Get pointer to a recipe by id
 */
Ingredient *drinks_io_get_ingredient_by_id(uint16_t id);

/**
 * @brief Read all ingredients from ingredients file and save them in
 * ingredients list store
 * @param ingArray array of ingredients
 * @return OK on success, ERROR otherwise
 */
VLArray_t *drinks_io_read_ingredients(void);

/**
 * @brief Get number of recipes read in from file
 */
uint32_t drinks_io_get_num_recipes(void);

/**
 * @brief Get pointer to a recipe by id
 */
Recipe *drinks_io_get_recipe_by_id(uint16_t id);

/**
 * @brief Read all recipes from recipes file and save them in recipe list store
 * @param recArray array of recipes
 * @return 0 on success, 1 otherwise
 */
VLArray_t *drinks_io_read_recipes(void);

#endif //DRINKS_H
