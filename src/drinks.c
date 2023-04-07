/* This file is the main implementation for the Drinks and Recipes Management of the ultramat.
 * It consists of reading the resource files and extracting the data into recipes/ingredients arrays.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "checks.h"
#include "drinks.h"
#include "drinkarray.h"

#define INPUT_BUFFER_SIZE 128
const char * const ingredientsFile = "res/ingredients.txt";
const char * const recipeFile = "res/recipes.txt";

/* private function declarations */

static uint8_t drinks_convert_to_number(char *buffer, int start, int end);

static void drinks_read_ingredients_from_file(Ing_Array_t *array);
static int drinks_get_ingredient_count();

static void drinks_read_recipes_from_file(Rec_Array_t *array);
static uint8_t drinks_read_recipe_ingredients(DrinkTuple_t *recipeIngredients, char *buffer);
static int drinks_get_recipe_count();

/* function definitions */

/**
 * @brief Read all ingredients from file and save them in array 
 * 
 * @return Ing_Array_t - array of ingredients 
*/
Ing_Array_t *get_all_ingredients()
{
    int ingredientCount = drinks_get_ingredient_count();

    Ing_Array_t *ingredientsArray = create_ing_array(ingredientCount);
    drinks_read_ingredients_from_file(ingredientsArray);

    return ingredientsArray;
}

/**
 * @brief Read all recipes from file and save them in array 
 * 
 * @return Rec_Array_t - array of recipes 
*/
Rec_Array_t *get_all_recipes()
{
    int recipeCount = drinks_get_recipe_count();

    Rec_Array_t *recipesArray = create_rec_array(recipeCount);
    drinks_read_recipes_from_file(recipesArray);

    return recipesArray;
}

/**
 * @brief get all ingredients from file and save them in ingredient array
 * 
 * @param array stores all ingredients with name and id
 */
void drinks_read_ingredients_from_file(Ing_Array_t *array)
{
    char buffer[INPUT_BUFFER_SIZE];
    memset(buffer, 0, INPUT_BUFFER_SIZE);
    int index = 0, lastSemiColon = 0;
    bool nameRead = false;

    FILE *inputFile = fopen(ingredientsFile, "r");
    CHECK_FILE(inputFile, "ingredients.txt");

    while(fgets(buffer, INPUT_BUFFER_SIZE, inputFile))
    {
        for(int i = 0; buffer[i] != '\n'; i++)
        {
            if(buffer[i] == ';')
            {
                if(!nameRead)
                {
                    char *name = (char *)calloc(i,sizeof(char));
                    strncpy(name, buffer, i);
                    array->ingredients[index].name = name;
                    nameRead = true;
                    lastSemiColon = i;
                }
                else
                {
                    uint8_t id = drinks_convert_to_number(buffer, lastSemiColon + 1, i);
                    array->ingredients[index].id = id;
                }
            }
        }
        array->ingredients[index].selected = false;
        array->ingredients[index].position = 0;

        index++;
        lastSemiColon = 0;
        nameRead = false;
    }

    fclose(inputFile);
}

/**
 * @brief get all recipes from file and save them in recipe array
 * 
 * @param array stores all recipes with given parameters
 */
void drinks_read_recipes_from_file(Rec_Array_t *array)
{
    char buffer[INPUT_BUFFER_SIZE];
    memset(buffer, 0, INPUT_BUFFER_SIZE);
    int index = 0, lastSemiColon = 0;
    bool nameRead = false, idRead = false;

    FILE *inputFile = fopen(recipeFile, "r");
    CHECK_FILE(inputFile, "recipes.txt");

    while(fgets(buffer, INPUT_BUFFER_SIZE, inputFile))
    {
        // allocate new recipe-ingredient list for current recipe
        DrinkTuple_t *currentRecipeList = (DrinkTuple_t *)malloc(sizeof(DrinkTuple_t));
        CHECK_ALLOC(currentRecipeList);
        uint8_t currentIngredientCount = 0;

        for(int i = 0; buffer[i] != '\n'; i++)
        {
            if(strcmp(buffer, "\n") == 0)
                break;

            if(buffer[i] == ';')
            {
                if(!nameRead)
                {
                    char *name = (char *)calloc(i, sizeof(char));
                    strncpy(name, buffer, i);
                    array->recipes[index].name = name;
                    nameRead = true;
                    // save semicolon for indexing id and ing-list
                    lastSemiColon = i; 
                }
                else
                {
                    if(!idRead)
                    {
                        uint8_t id = drinks_convert_to_number(buffer, lastSemiColon + 1, i);
                        array->recipes[index].id = id;
                        idRead = true;
                        lastSemiColon = i;
                    }
                    else
                    {
                        // enter only after reading both name and id
                        // pass string beginning !after! name and id
                        currentIngredientCount =
                                drinks_read_recipe_ingredients(currentRecipeList, &buffer[lastSemiColon]);
                        // read next recipe after done
                        break;
                    }
                }
            }
        }
        array->recipes[index].recipeTuples = currentRecipeList;
        array->recipes[index].ingredientCount = currentIngredientCount;
        array->recipes[index].available = false;     

        index++;
        nameRead = false;
        idRead = false;
    }

    fclose(inputFile);
}

/**
 * @brief Saves id and quantity of each recipe-ingredient in tuple-array
 * 
 * @param recipeIngredients Array of tuples, id and quantity (for each ingredient)
 * @param buffer input string after recipe name and id
 * @return uint8_t ingredient count for the recipe 
 */
uint8_t drinks_read_recipe_ingredients(DrinkTuple_t *recipeIngredients, char *buffer)
{
    int lastHit = 0, digitCount = 0;
    int ingCounter = 1, ingIndex = 0;

    uint8_t ingId = 0, ingQuantity = 0;

    // array holds each ingredient id with given quantity
    DrinkTuple_t *recipeTuples = recipeIngredients;

    for(int i = 0; buffer[i] != '\n'; ++i)
    {
        if(buffer[i] == ';' && buffer[i+1] != '\n')
        {
            lastHit = i + 1; // jump over SemiColon
            for(int j = lastHit; buffer[j] != '-'; ++j)
            {
                digitCount++;
            }

            ingId = drinks_convert_to_number(buffer, lastHit, lastHit + digitCount);
            recipeTuples[ingIndex].id = ingId;

            lastHit += digitCount;
            digitCount = 0;
        }
        if(buffer[i] == '-')
        {
            lastHit = i + 1; // jump over Dash
            for(int j = lastHit; buffer[j] != ';'; ++j)
                digitCount++;

            ingQuantity = drinks_convert_to_number(buffer, lastHit, lastHit + digitCount);
            recipeTuples[ingIndex].quantity = ingQuantity;            
            ingIndex++;
            ingCounter++;

            lastHit += digitCount;
            digitCount = 0;
        }
    }

    recipeIngredients = recipeTuples;

    return (uint8_t)ingIndex;
}

/**
 * @brief Get the count of all ingredients from ingredients.txt
 * 
 * @return int - count
 */
int drinks_get_ingredient_count()
{
    int ingredientCount = 0;
    char buffer[INPUT_BUFFER_SIZE];
    memset(buffer, 0, INPUT_BUFFER_SIZE);
    
    FILE *inputFile = fopen(ingredientsFile, "r");
    CHECK_FILE(inputFile, "ingredients.txt");

    while(fgets(buffer, INPUT_BUFFER_SIZE, inputFile))
    {
        ingredientCount++;
    }

    fclose(inputFile);
    return ingredientCount;
}

/**
 * @brief Get the count of all recipes from recipes.txt
 * 
 * @return int - recipe count
 */
int drinks_get_recipe_count()
{
    int recipeCount = 0;
    char buffer[INPUT_BUFFER_SIZE];
    memset(buffer, 0, INPUT_BUFFER_SIZE);
    
    FILE *inputFile = fopen(recipeFile, "r");
    CHECK_FILE(inputFile, "recipes.txt");

    while(fgets(buffer, INPUT_BUFFER_SIZE, inputFile))
    {
        recipeCount++;
    }

    fclose(inputFile);
    return recipeCount;
}

/**
 * @brief convert a number as a char to an integer (multi digit possible)
 * 
 * @param buffer whole string with char numbers
 * @param start start of specific char number
 * @param end end of specific char number
 * @return uint8_t converted number
 */
static uint8_t
drinks_convert_to_number(char *buffer, int start, int end)
{
    int convertedNumber = 0;
    int digitCount = end - start;
    
    char *strNumber = (char *)calloc(sizeof(char), digitCount);
    CHECK_ALLOC(strNumber);

    strncpy(strNumber, &buffer[start], digitCount);
    convertedNumber = atoi(strNumber);
    free(strNumber);

    return convertedNumber;
}