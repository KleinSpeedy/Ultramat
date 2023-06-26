/**
 * Source file for ingredient object
 */

/* Module include */
#include "ingredient.h"

struct _UIngredient
{
    GObject parent_instance;

    const gchar *name;        /* Name of ingredient */
    guint id;           /* ingredient identifier */
    gint8 position;     /* position on axis, needs to be signed int so -1 can be initial value */
    gboolean selected;  /* ingredient currently selected or not */
};

// Ingredient currently holds no position
#define NO_POSITION -1

G_DEFINE_TYPE (UIngredient, u_ingredient, G_TYPE_OBJECT)

static void
u_ingredient_class_init(UIngredientClass *class)
{
    (void) class;
}

static void
u_ingredient_init(UIngredient *self)
{
    // Default values
    self->name = g_strdup("");
    self->id = 0;
    self->position = NO_POSITION; // because we have a position zero set it to -1
    self->selected = FALSE; // initially nothing is selected
}

guint
u_ingredient_get_id(UIngredient *self)
{
    if(!self)
        return 0;
    return self->id;
}

void
u_ingredient_set_id(UIngredient *self, guint id)
{
    if(!self)
        return;
    self->id = id;
}

gint8
u_ingredient_get_position(UIngredient *self)
{
    if(!self)
        return 0;
    return self->position;
}
void
u_ingredient_set_position(UIngredient *self, gint8 pos)
{
    if(!self)
        return;
    self->position = pos;
}
/**
 * @brief Sets position to -1 and selected to FALSE, indicating that
 * the ingredient has been unselected
 * @param self Ingredient
 */
void
u_ingredient_set_unselected(UIngredient *self)
{
    u_ingredient_set_position(self, NO_POSITION);
    u_ingredient_set_selected(self, FALSE);
}

/**
 * @brief Sets new Ingredient position and selected to TRUE
 * @param self instance
 * @param position new position
 */
void
u_ingredient_is_selected(UIngredient *self, gint8 position)
{
    u_ingredient_set_position(self, position);
    u_ingredient_set_selected(self, TRUE);
}

gboolean
u_ingredient_get_selected(UIngredient *self)
{
    if(!self)
    {
        g_log("Ingredients", G_LOG_LEVEL_WARNING, "Object for selected is null!");
        return FALSE;
    }
    return self->selected;
}
void
u_ingredient_set_selected(UIngredient *self, gboolean toSet)
{
    if(!self)
        return;
    self->selected = toSet;
}

const gchar *
u_ingredient_get_name(UIngredient *self)
{
    if(!self)
        return "";
    return self->name;
}
void
u_ingredient_set_name(UIngredient *self, const gchar *name)
{
    if(g_strcmp0(name, self->name) == 0)
    {
        g_free((gchar *)self->name);
        self->name = g_strdup(name);
    }
}

UIngredient  *
u_ingredient_new(const gchar *name, guint id)
{
    UIngredient *ing = NULL;
    ing = g_object_new(U_TYPE_INGREDIENT, NULL);

    ing->name = g_strdup(name);
    g_free((gchar *)name);
    ing->id = id;

    return ing;
}

#ifdef ULTRA_DEBUG
#include <stdio.h>

void
dbg_print_ingredient(UIngredient *ing)
{
    if(!ing)
    {
        fprintf(stderr, "Object is null!\n");
    }
    printf("Name: %s ID: %d Pos: %d Sel: %d\n",
           ing->name,
           ing->id,
           ing->position,
           ing->selected);
}

#endif // ULTRA_DEBUG