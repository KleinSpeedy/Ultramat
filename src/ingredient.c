/**
 * Source file for ingredient object
 */

/* Module include */
#include "ingredient.h"

struct _UIngredient
{
    GObject parent_instance;

    gchar *name;        /* Name of ingredient */
    guint id;           /* ingredient identifier */
    guint8 position;    /* position on axis */
    gboolean selected;  /* ingredient currently selected or not */
};

G_DEFINE_TYPE (UIngredient, u_ingredient, G_TYPE_OBJECT)

static void
u_ingredient_class_init(UIngredientClass *class)
{
}

static void
u_ingredient_init(UIngredient *self)
{
    // Default values
    self->name = g_strdup("");
    self->id = 0;
    self->position = 0;
    self->selected = FALSE;
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

guint8
u_ingredient_get_position(UIngredient *self)
{
    if(!self)
        return 0;
    return self->position;
}
void
u_ingredient_set_position(UIngredient *self, guint8 pos)
{
    if(!self)
        return;
    self->position = pos;
}

gboolean
u_ingredient_get_selected(UIngredient *self)
{
    if(!self)
        return FALSE;
    return self->selected;
}
void
u_ingredient_set_selected(UIngredient *self, gboolean toSet)
{
    if(!self)
        return;
    self->selected = toSet;
}

const
gchar *u_ingredient_get_name(UIngredient *self)
{
    if(!self)
        return "";
    return self->name;
}
void
u_ingredient_set_name(UIngredient *self, gchar *name)
{
    if(g_strcmp0(name, self->name) == 0)
    {
        g_free(self->name);
        self->name = g_strdup(name);
    }
}

UIngredient  *
u_ingredient_new(gchar *name, guint id, guint8 position, gboolean selected)
{
    UIngredient *ing;
    ing = g_object_new(U_TYPE_INGREDIENT, NULL);

    ing->name = g_strdup(name);
    g_free(name);
    ing->id = id;
    ing->position = position;
    ing->selected = selected;
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