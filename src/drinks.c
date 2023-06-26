/* This file is the main implementation for the Drinks and Recipes Management of the ultramat.
 * It consists of reading the resource files and extracting the data into recipes/ingredients arrays.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "checks.h"
#include "drinks.h"
#include "drinklists.h"

#define INPUT_BUFFER_SIZE 128

const char * const ingFileStr = "res/ingredients.txt";
const char * const recFileStr = "res/recipes.txt";

/* ========== HELPER FUNCTIONS ========== */

/**
 * @brief convert a number as a char to an integer (multi digit possible)
 * 
 * @param buffer whole string with char numbers
 * @param start start of specific char number
 * @param end end of specific char number
 * @return guint converted number
 */
static guint
drinks_str_to_number(const gchar *buffer, guint start, guint end)
{
    guint convertedNumber = 0;
    guint digitCount = end - start;
    
    gchar *strNumber = g_malloc0(digitCount);
    CHECK_ALLOC(strNumber);

    strncpy(strNumber, &buffer[start], digitCount);
    convertedNumber = atoi(strNumber);

    g_free(strNumber);
    return convertedNumber;
}

/* ========== STATIC FUNCTIONS ========== */

static FILE *
drinks_open_ingredients_file()
{
    FILE *fp = fopen(ingFileStr, "r");
    CHECK_FILE(fp, ingFileStr);
    return fp;
}

static void
drinks_close_ingredients_file(FILE *fp)
{
    fclose(fp);
}

/**
 * @brief Read the name of the ingredient and save the offset in buffer for reading ID
 * @param buffer file buffer
 * @param bufferIndex offset in buffer
 * @return name of ingredient
 */
static gchar *
drinks_ingredient_read_name(gchar *buffer, guint *bufferIndex)
{
    for(guint i = 0; buffer[i] != '\n'; ++i)
    {
        if(buffer[i] == ';')
        {
            // Save position for reading ID
            *bufferIndex = i;
            return g_strndup(buffer, i);
        }
    }
    // Error reading name!
    return 0;
}
/**
 * @brief Read the ID of the ingredient after the ';'
 * @param buffer file buffer
 * @param fileIndex offset in buffer
 * @return ID of ingredient
 */
static guint
drinks_ingredient_read_id(const gchar *buffer, guint fileIndex)
{
    for(guint i = fileIndex; buffer[i] != '\n'; ++i)
    {
        if(buffer[i] == ';')
        {
            return drinks_str_to_number(buffer, fileIndex, i);;
        }
    }
    // TODO: Error handling
    return 0;
}

static FILE *
drinks_open_recipes_file()
{
    FILE *fp = fopen(recFileStr, "r");
    CHECK_FILE(fp, recFileStr);
    return fp;
}

static void
drinks_close_recipes_file(FILE *fp)
{
    fclose(fp);
}

/**
 * @brief Read the name of the recipe and save the offset for the ID
 * @param buffer file buffer
 * @param bufferIndex offset in buffer
 * @return name of recipe
 */
static const gchar *
drinks_recipe_read_name(const gchar *buffer, guint *bufferIndex)
{
    for(guint i = 0; buffer[i] != '\n'; ++i)
    {
        if(buffer[i] == ';')
        {
            // save position
            *bufferIndex = i;
            return g_strndup(buffer, i);
        }
    }
    return NULL;
}
/**
 * @brief Read the ID of the recipe from the buffer
 * @param buffer file buffer
 * @param fileIndex offset in buffer
 * @return ID of recipe
 */
static guint
drinks_recipe_read_id(const gchar *buffer, guint fileIndex)
{
    for(guint i = fileIndex; buffer[i] != '\n'; ++i)
    {
        if(buffer[i] == ';')
        {
            return drinks_str_to_number(buffer, fileIndex, i);;
        }
    }
    // TODO: Error handling
    return 0;
}

/**
 * @brief Read the list of ingredients for each recipe
 *  Ingredients are separated by ';' and each ingredient has a quantity (number after '-')
 * @param buffer file buffer
 * @param rec Recipe for ingredients list
 */
static void
drinks_recipe_read_ingredients(const char *buffer, URecipe *rec)
{
    gboolean idRead = FALSE, quantityRead = FALSE;
    guint lastHit = 0, digitCount = 0;
    guint ingID = 0, ingQuantity = 0;

    for(guint i = 0; buffer[i] != '\n'; ++i)
    {
        if(buffer[i] == ';' && buffer[i+1] != '\n')
        {
            lastHit = i + 1; // jump over SemiColon
            for(guint j = lastHit; buffer[j] != '-'; ++j)
                digitCount++;

            ingID = drinks_str_to_number(buffer, lastHit, lastHit + digitCount);
            // reset digit count for next number
            digitCount = 0;
            idRead = TRUE;
        }
        if(buffer[i] == '-')
        {
            lastHit = i + 1; // jump over Dash
            for(guint j = lastHit; buffer[j] != ';'; ++j)
                digitCount++;

            ingQuantity = drinks_str_to_number(buffer, lastHit, lastHit + digitCount);
            // reset digit count for next number
            digitCount = 0;
            quantityRead = TRUE;
        }
        if(idRead && quantityRead)
        {
            // Get ingredient pointer from ingredient ID and append it to GList
            UIngredient *ing = lists_ingredient_get_by_id(ingID);
            if(!u_recipe_append_ingredient(rec, ing, ingQuantity))
                g_print("Error appending ingredient to recipe!");
            // Reset status booleans
            idRead = FALSE;
            quantityRead = FALSE;
        }
    }
}

/* ========== NON-STATIC FUNCTIONS ========== */

/**
 * @brief Read all ingredients from ingredients file and save them in ingredients list store
 *
 * @return OK on success, ERROR otherwise
 */
InputStatus_t
drinks_io_read_ingredients()
{
    FILE *input = drinks_open_ingredients_file();
    gchar buffer[INPUT_BUFFER_SIZE];
    memset(buffer, 0, INPUT_BUFFER_SIZE);

    while(fgets(buffer, INPUT_BUFFER_SIZE, input))
    {
        // TODO: Make names const gchar *
        gchar *name;
        guint id, bufferIndex = 0;

        if(!(name = drinks_ingredient_read_name(buffer, &bufferIndex)))
            return DRINKS_ERROR;
        // +1 skips semicolon | TODO: Error handling reading id
        id = drinks_ingredient_read_id(buffer, bufferIndex+1);

        // Create new ingredient object
        UIngredient *newIng = u_ingredient_new(name, id);

        lists_ingredient_append(newIng);
    }

    drinks_close_ingredients_file(input);
    return DRINKS_OK;
}

/**
 * @brief Read all recipes from recipes file and save them in recipe list store
 *
 * @return OK on success, ERROR otherwise
 */
InputStatus_t
drinks_io_read_recipes()
{
    FILE *input = drinks_open_recipes_file();
    gchar buffer[INPUT_BUFFER_SIZE];
    memset(buffer, 0, INPUT_BUFFER_SIZE);

    while(fgets(buffer, INPUT_BUFFER_SIZE, input))
    {
        const gchar *name = NULL;
        guint id = 0, bufferIndex = 0;
        gboolean available = FALSE;

        if((name = drinks_recipe_read_name(buffer, &bufferIndex)) == NULL)
            return DRINKS_ERROR;
        id = drinks_recipe_read_id(buffer, bufferIndex+1);
        // Create a new recipe
        URecipe *newRec = u_recipe_new(name, id, available);
        // start reading list of ingredients for recipe
        drinks_recipe_read_ingredients(&buffer[bufferIndex], newRec);

        lists_recipe_append(newRec);
    }

    drinks_close_recipes_file(input);
    return DRINKS_OK;
}