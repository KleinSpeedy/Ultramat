//
// Source file for recipe object
//

// Module include
#include "recipe.h"
#include <stdlib.h>
#include <glib.h>

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

    gchar *name;        /* Recipe name */
    guint id;           /* Recipe identifier */
    gboolean available; /* Is recipe currently available or not */
    gboolean selected;  /* Is recipe currently selected or not, only one at a time! */

    guint ingCount;     /* Number of Ingredients */
    GList *ingredients; /* LinkedList with Ingredients information (actual recipe) */
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
}

// TODO: Provide destructor

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
        g_log(ULTRA_LOG,
              G_LOG_LEVEL_WARNING,
              "Allocating new RecipeInfo failed");
        return NULL;
    }
    temp->ing = ing;
    temp->quantity = quantity;
    return temp;
}

static void
u_recipe_info_delete(struct RecipeInfo *info)
{
    free(info);
}

/**
 * @brief GFunc for ingredients GList checking if all ingredients are selected
 * @param data RecipeInfo
 * @param userData available boolean
 */
// TODO: Use a mask leftShifted with 1s by ingCount and compare it to actual selected
static gboolean
u_recipe_list_check_available(GList *list, guint ingCount)
{
    if(!list)
        return FALSE;
    GList *temp = list;
    struct RecipeInfo *data = (struct RecipeInfo *)list->data;
    gboolean ret = FALSE;
    if(!data)
    {
        g_print("Availability: NULL\n");
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

static void
u_recipe_list_check_available_fe(gpointer data, gpointer user_data)
{
    if(!data)
    {
        g_print("RecipeInfo is NULL!\n");
        return;
    }
    gboolean selected = GPOINTER_TO_INT(user_data);
    struct RecipeInfo *temp = (struct RecipeInfo *)data;
    g_print("RI: %s %d -> %d\n",
            u_ingredient_get_name(temp->ing),
            temp->quantity,
            u_ingredient_get_selected(temp->ing));
    if(u_ingredient_get_selected(temp->ing))
    {
        selected = TRUE;
    }
    else
    {
        selected = FALSE;
    }
    user_data = GINT_TO_POINTER(selected);
}

/**
 * @brief Iterate over all ingredients and check if available
 * @param rec Recipe
 * @return TRUE if available, FALSE otherwise
 */
static gboolean
u_recipe_check_available(URecipe *rec)
{
    return u_recipe_list_check_available(rec->ingredients, rec->ingCount);
}

/* ========== Public ========== */

gchar *
u_recipe_get_name(URecipe *self)
{
    if(!self)
        return "";
    return self->name;
}

void
u_recipe_set_name(URecipe *self, gchar *name)
{
    if(g_strcmp0(self->name, name) == 0)
    {
        g_free(self->name);
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
    return u_recipe_check_available(self);
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
    rec->ingredients = g_list_append(rec->ingredients, u_recipe_info_new(ing, quantity));
    rec->ingCount++;

    return TRUE;
}

URecipe *
u_recipe_new(gchar *name, guint id, gboolean available)
{
    URecipe *rec = g_object_new(U_TYPE_RECIPE, NULL);

    rec->name = g_strdup(name);
    g_free(name);
    rec->id = id;
    rec->available = available;
    rec->ingredients = g_list_alloc();
    return rec;
}

#ifdef ULTRA_DEBUG
void
dbg_print_recipe_list(gpointer data, gpointer user_data)
{
    struct RecipeInfo *info = (struct RecipeInfo *)data;
    g_print("Name: %s, Quantity: %d\n",
            u_ingredient_get_name(info->ing),
            info->quantity);
}

void
dbg_print_recipe(URecipe *rec)
{
    if(!rec)
    {
        g_print("recipe is null!\n");
        return;
    }
    g_print("Name: %s, ID: %d, ICount: %d, Available: %d\n",
            rec->name,
            rec->id,
            rec->ingCount,
            rec->available);
    g_list_foreach(rec->ingredients, dbg_print_recipe_list, NULL);
}

#endif // ULTRA_DEBUG