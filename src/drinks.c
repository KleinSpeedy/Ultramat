/*
 * This file is the main implementation for the Drinks and Recipes Management of
 * the ultramat. It consists of reading the resource files and extracting the
 * data into recipes/ingredients arrays.
 */

#include "drinks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

static const char *const ingFileStr = "res/ingredients.txt";
static const char *const recFileStr = "res/recipes.txt";

static const char *const SEPARATOR = ";";

// TODO: Can this be done better ?
static VLArray_t ingArray;
static VLArray_t recArray;

// drinktype functions (dt)

// append an ingredient-count pair to recipe
static int dt_recipe_append_pair(Recipe *rec, uint32_t id, uint32_t count)
{
    const uint32_t nextSize = rec->ingCount + 1;

    rec->ingPairs = realloc(rec->ingPairs, sizeof(*rec->ingPairs) * nextSize);
    if(!rec->ingPairs)
        return 1;

    rec->ingPairs[rec->ingCount].id = id;
    rec->ingPairs[rec->ingCount].count = count;
    rec->ingCount++;
    return 0;
}

// TODO: error check possible?
static void ingredient_create(char *input, Ingredient *ing)
{
    int name_done = 0;

    char *part_str = strtok(input, SEPARATOR);
    while(part_str != NULL)
    {
        // TODO: Get rid of stupid bool check
        if(name_done == 0)
        {
            strncpy(ing->name, part_str, MAX_NAME_LENGTH);
            name_done = 1;
        }
        else
        {
            ing->id = atoi(part_str);
            break;
        }
        part_str = strtok(NULL, SEPARATOR);
    }
}

static void recipe_create(char *input, Recipe *rec)
{
    char *part_str = strtok(input, SEPARATOR);
    strncpy(rec->name, part_str, MAX_NAME_LENGTH);

    part_str = strtok(NULL, SEPARATOR);
    rec->id = atoi(part_str);

    // after reading recipe id only id count pairs remain
    part_str = strtok(NULL, "-");

    // TODO: This can be done better
    uint16_t ing_id = 0;
    uint8_t ing_count = 0;
    uint8_t id_or_count = 0;
    while(part_str != NULL)
    {
        if(id_or_count == 0)
        {
            ing_id = atoi(part_str);
            part_str = strtok(NULL, SEPARATOR);
            id_or_count = 1;
        }

        if(part_str == NULL)
        {
            // TODO: Proper error log
            fprintf(stderr, "Error during id count pair decoding\n");
            return;
        }

        if(id_or_count == 1)
        {
            ing_count = (uint8_t)atoi(part_str);
            part_str = strtok(NULL, "-");
            id_or_count = 0;

            if(dt_recipe_append_pair(rec, ing_id, ing_count) != 0)
            {
                // TODO: Proper error log
                fprintf(stderr, "Error during id count pair decoding\n");
                return;
            }
        }
        // empty string with newline only is left at the end
        if(strcmp(part_str, "\n") == 0)
        {
            break;
        }
    }
}

Ingredient *drinks_io_get_ingredient_by_id(uint16_t id)
{
    Ingredient *ing = NULL, *temp = NULL;
    for(size_t i = 0; i < ingArray.used; i++)
    {
        // TODO: Id is index in array, so we could use this function directly?
        temp = vla_get_elem_at(&ingArray, i);
        if(temp->id == id)
        {
            ing = temp;
            break;
        }
    }

    return ing;
}

VLArray_t *drinks_io_read_ingredients(void)
{
    vla_init(&ingArray, sizeof(Ingredient));
    char inputBuffer[BUFFER_SIZE] = {0};

    FILE *fp = fopen(ingFileStr, "r");
    if(fp == NULL)
    {
        // TODO: Error log
        return NULL;
    }

    while(fgets(inputBuffer, BUFFER_SIZE, fp))
    {
        struct Ingredient ing = {0};
        ingredient_create(inputBuffer, &ing);

        if(vla_append(&ingArray, &ing) != 0)
        {
            fclose(fp);
            return NULL;
        }
        memset(inputBuffer, 0, BUFFER_SIZE);
    }

    fclose(fp);
    return &ingArray;
}

Recipe *drinks_io_get_recipe_by_id(uint16_t id)
{
    Recipe *rec = NULL, *temp = NULL;
    for(size_t i = 0; i < recArray.used; i++)
    {
        temp = vla_get_elem_at(&recArray, i);
        if(temp->id == id)
        {
            rec = temp;
            break;
        }
    }

    return rec;
}

VLArray_t *drinks_io_read_recipes(void)
{
    vla_init(&recArray, sizeof(Recipe));
    char input_buffer[BUFFER_SIZE] = {0};

    FILE *fp = fopen(recFileStr, "r");
    if(fp == NULL)
    {
        // TODO: Error log
        return NULL;
    }

    while(fgets(input_buffer, BUFFER_SIZE, fp))
    {
        struct Recipe rec = {0};
        recipe_create(input_buffer, &rec);
        // dont bother to free as program will exit anyway
        if(vla_append(&recArray, &rec) != 0)
        {
            // TODO: Error log, what about allocated objects?
            fclose(fp);
            return NULL;
        }
        memset(input_buffer, 0, sizeof(input_buffer));
    }

    fclose(fp);
    return &recArray;
}

void drinks_io_recipe_custom_free(void *data)
{
    struct Recipe *r = data;
    if(r->ingPairs != NULL)
    {
        free(r->ingPairs);
        r->ingPairs = NULL;
    }
}
