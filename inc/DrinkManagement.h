#ifndef __DRINK_MANAGE_H__
#define __DRINK_MANAGE_H__

/*--- INCLUDES ---*/
#include <gtk/gtk.h>

/*--- METHODS ---*/

void setupRecipesAndDrinks();

/*--- STRUCTURES ---*/

/**
 * @brief Represents a drink
 */
typedef struct Drink_t
{
    char *drinkName;
    uint8_t position;
    uint8_t id;
    gboolean selected;
} Drink_t;

/**
 * @brief Linked list of Drinks for one recipe
 */
typedef struct Recipe_t
{
    struct Recipe_t *next;
    Drink_t drink;

    char *recipeName;
    gboolean available;
} Recipe_t;

#endif //__DRINK_MANAGE_H__