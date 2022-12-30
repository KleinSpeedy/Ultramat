#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inc/drinks.h"
#include "inc/drinkarray.h"

#define INPUT_BUFFER 128
const char *ingredientsFile = "ingredients.txt";
const char *recipeFile = "recipes.txt";

/* private function declarations */

uint8_t convertToNumber(char *buffer, int start, int end);

void readIngredientsFromFile(Ing_Array_t *array);
int getIngredientCount();

void readRecipesFromFile(Rec_Array_t *array);
uint8_t readRecipeIngredients(DrinkTuple_t *recipeIngredients, char *buffer);
int getRecipeCount();

/* function definitions */

Ing_Array_t *getAllIngredients()
{
    int ingredientCount = getIngredientCount();

    Ing_Array_t *ingredientsArray = create_ing_array(ingredientCount);
    readIngredientsFromFile(ingredientsArray);

    return ingredientsArray;
}

Rec_Array_t *getAllRecipes()
{
    int recipeCount = getRecipeCount();

    Rec_Array_t *recipesArray = create_rec_array(recipeCount);
    readRecipesFromFile(recipesArray);

    return recipesArray;
}

void setupDrinkManagement(Ing_Array_t *ingredientsArray, Rec_Array_t *recipesArray)
{
    int ingredientCount = getIngredientCount();
    int recipeCount = getRecipeCount();

    ingredientsArray = create_ing_array(ingredientCount);
    readIngredientsFromFile(ingredientsArray);
    /*
    for(int i = 0; i < ingredientsArray->size; i++)
    {
        Ingredient_t current = get_at_ing_array(ingredientsArray, i);
        printf("name: %s id: %d pos: %d selected: %d\n",
            current.name,
            current.id,
            current.position,
            current.selected);
    }
    */

    recipesArray = create_rec_array(recipeCount);
    readRecipesFromFile(recipesArray);
    /*
    for(int i = 0; i < recipesArray->size; i++)
    {
        Recipe_t current = get_at_rec_array(recipesArray, i);
        printf("name: %s id: %d ing: %d\n",current.name, current.id, current.ingredientCount);
        for(int j = 0; j < current.ingredientCount; j++)
        {
            printf("id: %d q: %d ",
                current.recipeTuples[j].id,
                current.recipeTuples[j].quantity);
        }
        printf("\n");
    }
    */
    //free_ing_array(ingredientsArray);
    //free_rec_array(recipesArray);
}

/**
 * @brief get all ingredients from file and save them in ingredient array
 * 
 * @param array stores all ingredients with name and id
 */
void readIngredientsFromFile(Ing_Array_t *array)
{
    char buffer[INPUT_BUFFER];
    memset(buffer, 0, INPUT_BUFFER);
    int index = 0, lastSemiColon = 0;
    bool nameRead = false;

    FILE *inputFile = fopen(ingredientsFile, "r");
    CHECK_FILE(inputFile, "ingredients.txt");

    while(fgets(buffer, INPUT_BUFFER, inputFile))
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
                    uint8_t id = convertToNumber(buffer, lastSemiColon+1, i);
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
void readRecipesFromFile(Rec_Array_t *array)
{
    char buffer[INPUT_BUFFER];
    memset(buffer, 0, INPUT_BUFFER);
    int index = 0, lastSemiColon = 0;
    bool nameRead = false, idRead = false;

    FILE *inputFile = fopen(recipeFile, "r");
    CHECK_FILE(inputFile, "recipes.txt");

    while(fgets(buffer, INPUT_BUFFER, inputFile))
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
                    // save semi colon for indexing id and ing-list
                    lastSemiColon = i; 
                }
                else
                {
                    if(!idRead)
                    {
                        uint8_t id = convertToNumber(buffer, lastSemiColon+1, i);
                        array->recipes[index].id = id;
                        idRead = true;
                        lastSemiColon = i;
                    }
                    else
                    {
                        // enter only after reading both name and id
                        // pass string beginning !after! name and id
                        currentIngredientCount =
                            readRecipeIngredients(currentRecipeList, &buffer[lastSemiColon]);
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
uint8_t readRecipeIngredients(DrinkTuple_t *recipeIngredients, char *buffer)
{
    bool firstIngredient = true;
    int lastHit = 0, digitCount = 0;
    int ingCounter = 1, ingIndex = 0;

    uint8_t ingId = 0, ingQuantity = 0;

    // array holds each ingredient id with given quantity
    DrinkTuple_t *recipeTuples = recipeIngredients;
    DrinkTuple_t *temp;

    for(int i = 0; buffer[i] != '\n'; i++)
    {
        if(buffer[i] == ';' && buffer[i+1] != '\n')
        {
            lastHit = i + 1; // jump over SemiColon
            for(int j = lastHit; buffer[j] != '-'; j++)
                digitCount++;
            
            ingId = convertToNumber(buffer, lastHit, lastHit+digitCount);

            // if not first ingredient reallocate new space
            if(!firstIngredient)
            {
                temp = (DrinkTuple_t *)realloc(recipeTuples, sizeof(DrinkTuple_t) * ingCounter);
                CHECK_ALLOC(temp);
                recipeTuples = temp;
                recipeTuples[ingIndex].id = ingId;
            }
            else
            {
                recipeTuples[ingIndex].id = ingId;
            }

            lastHit += digitCount;
            digitCount = 0;
        }
        if(buffer[i] == '-')
        {
            lastHit = i + 1; // jump over Dash
            for(int j = lastHit; buffer[j] != ';'; j++)
                digitCount++;
            
            ingQuantity = convertToNumber(buffer, lastHit, lastHit+digitCount);
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
int getIngredientCount()
{
    int ingredientCount = 0;
    char buffer[INPUT_BUFFER];
    memset(buffer, 0, INPUT_BUFFER);
    
    FILE *inputFile = fopen(ingredientsFile, "r");
    CHECK_FILE(inputFile, "ingredients.txt");

    while(fgets(buffer, INPUT_BUFFER, inputFile))
    {
        ingredientCount++;
    }

    fclose(inputFile);
    return ingredientCount;
}

/**
 * @brief Get the count of all recipes from recipes.txt
 * 
 * @return int - count
 */
int getRecipeCount()
{
    int recipeCount = 0;
    char buffer[INPUT_BUFFER];
    memset(buffer, 0, INPUT_BUFFER);
    
    FILE *inputFile = fopen(recipeFile, "r");
    CHECK_FILE(inputFile, "recipes.txt");

    while(fgets(buffer, INPUT_BUFFER, inputFile))
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
uint8_t convertToNumber(char *buffer, int start, int end)
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