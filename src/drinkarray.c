/* This file implements the Recipes and Ingredients Array data structures 
 * Consists of creation, deletion, setting and deleting a object at a specific position */

#include <stdlib.h>
#include <stdio.h>
#include "checks.h"
#include "drinkarray.h"

/**
 * @brief Create a ingredients array of a specified size
 * 
 * @param size array size
 * @return Ing_Array_t*
 */
Ing_Array_t *create_ing_array(size_t size)
{
    Ing_Array_t *temp = (Ing_Array_t *)malloc(sizeof(Ing_Array_t));
    CHECK_ALLOC(temp);

    temp->ingredients = (Ingredient_t *)malloc(size * sizeof(Ingredient_t));
    CHECK_ALLOC(temp->ingredients);
    temp->size = size;

    return temp;
}

/**
 * @brief Frees up the ingredients array and names
 * 
 * @param array 
 */
void free_ing_array(Ing_Array_t *array)
{
    if(array)
    {
        for(size_t i = 0; i < array->size; i++)
        {
            free(array->ingredients[i].name);
            //free(array->ingredients[i].id);
        }
        free(array->ingredients);
        free(array);
    }
    else
    {
        fprintf(stderr,"Couldnt delete Array!\n");
        exit(1);
    }
}

/**
 * @brief Get the object at the specified position
 * 
 * @param array - array to look in
 * @param position - position that the object is at
 * @return Ingredient_t 
 */
Ingredient_t get_at_ing_array(Ing_Array_t *array, size_t position)
{
    if(array && position < array->size)
    {
        return array->ingredients[position];
    }
    else
    {
        fprintf(stderr, "Error accessing array\n");
        exit(1);
    }
}

/**
 * @brief Set a ingredient in the specified array at the specified position
 * 
 * @param array 
 * @param position 
 * @param ing 
 */
void set_at_ing_array(Ing_Array_t *array, size_t position, Ingredient_t ing)
{
    if(array && position < array->size)
    {
        array->ingredients[position] = ing;
    }
    else
    {
        fprintf(stderr, "Error accessing array\n");
        exit(1);
    }
}

/**
 * @brief Create a recipes array of specified size
 * 
 * @param size array size
 * @return Rec_Array_t* 
 */
Rec_Array_t *create_rec_array(size_t size)
{
    Rec_Array_t *temp = (Rec_Array_t *)malloc(sizeof(Rec_Array_t));
    CHECK_ALLOC(temp);

    temp->recipes = (Recipe_t *)malloc(size * sizeof(Recipe_t));
    CHECK_ALLOC(temp->recipes);
    temp->size = size;

    return temp;
}

/**
 * @brief Frees specified array, recipes (-names) and ingredient tuples
 * 
 * @param array 
 */
void free_rec_array(Rec_Array_t *array)
{
    if(array)
    {
        for(size_t i = 0; i < array->size; i++)
        {
            free(array->recipes[i].name);
            free(array->recipes[i].recipeTuples);
        }
        free(array->recipes);
        free(array);
    }
    else
    {
        fprintf(stderr,"Couldnt delete Array!\n");
        exit(1);
    }
}

/**
 * @brief Get the object at the specified position
 * 
 * @param array 
 * @param position 
 * @return Recipe_t 
 */
Recipe_t get_at_rec_array(Rec_Array_t *array, size_t position)
{
    if(array && position < array->size)
    {
        return array->recipes[position];
    }
    else
    {
        fprintf(stderr, "Error accessing array\n");
        exit(1);
    }
}

/**
 * @brief Set the recipe object at the specified position
 * 
 * @param array 
 * @param position 
 * @param ing 
 */
void set_at_rec_array(Rec_Array_t *array, size_t position, Recipe_t ing)
{
    if(array && position < array->size)
    {
        array->recipes[position] = ing;
    }
    else
    {
        fprintf(stderr, "Error accessing array\n");
        exit(1);
    }
}