//
// Source file for recipe object
//

// Module include
#include "recipe.h"
#include <stdlib.h>
#include <glib-object.h>

#define ULTRA_LOG "Recipes"

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

    const gchar *name;  /* Recipe name */
    guint id;           /* Recipe identifier */
    gboolean available; /* Is recipe currently available or not */
    gboolean selected;  /* Is recipe currently selected or not, only one at a time! */

    guint ingCount;     /* Number of Ingredients */
    GSList *ingredients; /* LinkedList with Ingredients information (actual recipe) */
};

G_DEFINE_TYPE(URecipe, u_recipe, G_TYPE_OBJECT)

static void
u_recipe_class_init(URecipeClass *class)
{
    (void) class;
}

static void
u_recipe_init(URecipe *rec)
{
    rec->name = g_strdup("");
    rec->id = 0;
    rec->ingCount = 0;
    rec->available = FALSE;
    rec->ingredients = NULL;
}

// TODO: Provide destructor

/**
 * @brief Search through whole list and check if ingredient is already part of the list
 * @param list list of ingredients
 * @param ing specific ingredient
 * @return TRUE if list already contains ingredient, false otherwise
 */
static gboolean
u_recipe_list_check_duplicate(GSList *list, UIngredient *ing)
{
    GSList *temp = g_slist_nth(list, 0);
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
        g_log(ULTRA_LOG,
              G_LOG_LEVEL_WARNING,
              "Allocating new RecipeInfo failed");
        return NULL;
    }
    temp->ing = ing;
    temp->quantity = quantity;
    return temp;
}

/**
 * @brief GFunc for ingredients GList checking if all ingredients are selected
 * @param data RecipeInfo
 * @param userData ingredient count
 */
// TODO: Use a mask leftShifted with 1s by ingCount and compare it to actual selected
static gboolean
u_recipe_list_check_available(GSList *list, guint ingCount)
{
    (void) ingCount;
    if(!list)
        return FALSE;

    GSList *temp = list;
    struct RecipeInfo *data = (struct RecipeInfo *)list->data;
    gboolean ret = FALSE;

    if(!data)
    {
        g_print("RecipeInfo: NULL\n");
        return FALSE;
    }

    for(; temp->next != NULL; temp = temp->next)
    {
        if(u_ingredient_get_selected(data->ing))
        {
            ret = TRUE;
        }
    }

    return ret;
}

/* ========== Public ========== */

const gchar *
u_recipe_get_name(URecipe *self)
{
    if(!self)
        return "";
    return self->name;
}

void
u_recipe_set_name(URecipe *self, const gchar *name)
{
    if(g_strcmp0(self->name, name) == 0)
    {
        g_free((gchar *)self->name);
        self->name = g_strdup(name);
    }
}

guint
u_recipe_get_id(URecipe *self)
{
    if(!self)
        return 0;
    return self->id;
}

void
u_recipe_set_id(URecipe *self, uint id)
{
    if(!self)
        return;
    self->id = id;
}

gboolean
u_recipe_is_available(URecipe *self)
{
    if(!self)
        return FALSE;
    return u_recipe_list_check_available(self->ingredients, self->ingCount);
}

void
u_recipe_set_available(URecipe *self, gboolean toSet)
{
    if(!self)
        return;
    self->available = toSet;
}

void
u_recipe_set_selected(URecipe *self, gboolean toSet)
{
    if(!self)
        return;
    self->selected = toSet;
}

gboolean
u_recipe_is_selected(URecipe *self)
{
    if(!self)
        return FALSE;
    return self->selected;
}

guint
u_recipe_get_ingredient_count(URecipe *self)
{
    if(!self)
        return 0;
    return self->ingCount;
}

gboolean
u_recipe_append_ingredient(URecipe *rec, UIngredient *ing, guint8 quantity)
{
    if(!rec || !ing)
    {
        g_log(ULTRA_LOG,
                G_LOG_LEVEL_WARNING,
                "Trying to append NULL objects to recipe list!");
        return FALSE;
    }
    rec->ingredients = g_slist_append(rec->ingredients, u_recipe_info_new(ing, quantity));
    rec->ingCount++;

    return TRUE;
}

URecipe *
u_recipe_new(const gchar *name, guint id, gboolean available)
{
    URecipe *rec = NULL;
    rec = g_object_new(U_TYPE_RECIPE, NULL);

    rec->name = g_strdup(name);
    g_free((char *)name);
    rec->id = id;
    rec->available = available;
    rec->ingredients = NULL;
    return rec;
}

/**
 * @brief Get quantity and current position for constructing serial buffer
 * @param rec active recipe
 * @param idx nth ingredient in list
 * @param pos current position of ingredient
 * @param quantity quantity of ingredient
 * @return TRUE on success | FALSE otherwise
 */
gboolean
u_recipe_get_nth_ingredient_info(URecipe *rec, guint idx, gint8 *pos, guint *quantity)
{
    if(rec == NULL)
        return FALSE;

    if(idx > u_recipe_get_ingredient_count(rec))
        return FALSE;

    struct RecipeInfo *info = NULL;
    if((info = g_slist_nth_data(rec->ingredients, idx)) == NULL)
        return FALSE;

    /* we don't check if position is initialized because it should be checked already */
    *pos = u_ingredient_get_position(info->ing);
    *quantity = info->quantity;

    return TRUE;
}

#ifdef ULTRA_DEBUG
void
dbg_print_recipe_list(gpointer data, gpointer user_data)
{
    (void) user_data;
    struct RecipeInfo *info = (struct RecipeInfo *)data;
    if(!info)
    {
        g_print("Dbg: RecipeInfo is null!\n");
        return;
    }
    g_print("Name: %s, Quantity: %d\n",
            u_ingredient_get_name(info->ing),
            info->quantity);
}

void
dbg_print_recipe(URecipe *rec)
{
    if(!rec)
    {
        g_print("Dbg: Recipe is null!\n");
        return;
    }
    g_print("Name: %s, ID: %d, ICount: %d, Available: %d\n",
            rec->name,
            rec->id,
            rec->ingCount,
            rec->available);
    g_slist_foreach(rec->ingredients, dbg_print_recipe_list, NULL);
}

#endif // ULTRA_DEBUG