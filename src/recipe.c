//
// Source file for recipe object
//

// Module include
#include "recipe.h"
#include <stdlib.h>
#include <glib/glist.h>

/* ===== Private ===== */

/* Holds information about quantity and used ingredient */
struct RecipeInfo
{
    UIngredient *ing;   /* Used UIngredient */
    guint8 quantity;    /* Liquor amount //TODO: Specify measurement */
};

struct _URecipe
{
    GObject *parent;

    gchar *name;        /* Recipe name */
    guint id;           /* Recipe identifier */
    gboolean available; /* Is recipe currently available or not */

    GList *ingredients; /* LinkedList with Ingredients information (actual recipe) */
};

G_DEFINE_TYPE(URecipe, u_recipe, G_TYPE_OBJECT)

static void
u_recipe_class_init(URecipeClass *class)
{
}

static void
u_recipe_init(URecipe *rec)
{
}

static gboolean
u_recipe_list_check_duplicate(GList *list, UIngredient *ing)
{
    GList *temp = g_list_first(list);
    for(; temp->next != NULL; temp = temp->next)
    {
        struct RecipeInfo *info = temp->data;
        if(info->ing == ing)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static struct RecipeInfo *
u_recipe_info_new(UIngredient *ing, guint8 quantity)
{
    struct RecipeInfo *temp = (struct RecipeInfo *) calloc(1, sizeof(struct RecipeInfo));
    if(!temp)
    {
        g_printerr("Allocating new RecipeInfo failed!");
        return NULL;
    }
    temp->ing = ing;
    temp->quantity = quantity;
    return temp;
}
/* ===== Public ===== */

gchar *
u_recipe_get_name(URecipe *self)
{
    g_assert_nonnull(self);
    if(!self)
        return "";
    return self->name;
}

void
u_recipe_set_name(URecipe *self, gchar *name)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->name = name;
}

guint
u_recipe_get_id(URecipe *self)
{
    g_assert_nonnull(self);
    if(!self)
        return 0;
    return self->id;
}
void

u_recipe_set_id(URecipe *self, uint id)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->id = id;
}

gboolean
u_recipe_is_available(URecipe *self)
{
    g_assert_nonnull(self);
    if(!self)
        return FALSE;
    return self->available;
}

void
u_recipe_set_available(URecipe *self, gboolean toSet)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->available = toSet;
}

gboolean
u_recipe_append_ingredient(URecipe *rec, UIngredient *ing, guint8 quantity)
{
    if(!rec || !ing)
    {
        g_printerr("Trying to access NULL objects!");
        return FALSE;
    }
    if(!u_recipe_list_check_duplicate(rec->ingredients, ing))
    {
        rec->ingredients = g_list_append(rec->ingredients, u_recipe_info_new(ing, quantity));
    }

    return TRUE;
}

URecipe *
u_recipe_new(gchar *name, guint id, gboolean available)
{
    URecipe *rec;
    rec = g_object_new(U_TYPE_RECIPE, NULL);
    g_assert_nonnull(rec);

    rec->name = name;
    rec->id = id;
    rec->available = available;
    rec->ingredients = NULL;    // Initialize this later on
    return rec;
}