/* This file implements creating GTK ListStore objects for recipes and ingredients
 * from their respective arrays */

#include "drinklists.h"
#include "checks.h"

#ifdef ULTRA_LOG
#define ULTRA_LOG ""
#endif // ULTRA_LOG
#define ULTRA_LOG "Drinklist"

// local ingredient list store holds all ingredient objects
static GtkListStore *ingListStore;
// local recipe list store holds all recipe objects
static GtkListStore *recListStore;

/* function implementation */


GtkListStore *
lists_ingredient_store()
{
    return ingListStore;
}

GtkListStore *
lists_recipe_store()
{
    return recListStore;
}

void
lists_create_ingredient_store()
{
    ingListStore = gtk_list_store_new(
            ING_NUM_COLUMN,
            G_TYPE_STRING,
            G_TYPE_OBJECT);
    CHECK_WIDGET(ingListStore, "ingredients list store");
}

void
lists_create_recipe_store()
{
    recListStore = gtk_list_store_new(
            REC_NUM_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_OBJECT);
    CHECK_WIDGET(recListStore, "Recipe List Store");
}

/**
 * @brief Appends new ingredient to ingredients store and saves name as well
 * @TODO Name gets copied, duplicate memory!
 * @param ing New ingredient
 */
void
lists_ingredient_append(UIngredient *ing)
{
    GtkTreeIter iter;
    const gchar *name = u_ingredient_get_name(ing);
    gtk_list_store_append (ingListStore, &iter);
    gtk_list_store_set (ingListStore, &iter,
                        ING_COLUMN_NAME, name,
                        ING_COLUMN_OBJECT, ing,
                        -1); // terminate
}

/**
 * @brief Search for an ingredient by its ID
 * @note Needs to be unrefed after use!
 * @param ingID id of wanted ingredient
 * @return Pointer to Ingredient with ID or NULL on failure
 */
UIngredient *
lists_get_ingredient_by_id(guint ingID)
{
    gboolean valid;

    GtkTreeModel *ingTreeModel = GTK_TREE_MODEL(ingListStore);
    GtkTreeIter iter;
    // get first object and search through store until id matches or if no more ingredients are stored
    valid = gtk_tree_model_get_iter_first(ingTreeModel, &iter);
    while(valid)
    {
        UIngredient *tempIng;
        gtk_tree_model_get(ingTreeModel, &iter,
                           ING_COLUMN_OBJECT, &tempIng,
                           -1); // terminate

        if(u_ingredient_get_id(tempIng) == ingID)
        {
            return tempIng;
        }
        valid = gtk_tree_model_iter_next(ingTreeModel, &iter);
    }
    // no ingredient with matching ID
    return NULL;
}

/**
 * @brief Appends new recipe to recipes store and saves name as well
 * @TODO Name gets copied, duplicate memory!
 * @param ing New recipe
 */
void
lists_recipe_append(URecipe *rec)
{
    GtkTreeIter iter;
    const gchar *name = u_recipe_get_name(rec);
    gtk_list_store_append (recListStore, &iter);
    gtk_list_store_set (recListStore, &iter,
                        REC_COLUMN_NAME, name,
                        REC_COLUMN_OBJECT, rec,
                        -1); // terminate
}

/* DEBUG FUNCTIONS */
#ifdef ULTRA_DEBUG
void
dbg_print_ingredients()
{
    g_log(ULTRA_LOG,
          G_LOG_LEVEL_INFO,
          "Start dbg print of ingredients:");
    GtkTreeModel *model = GTK_TREE_MODEL(ingListStore);
    GtkTreeIter iter;
    gboolean valid;

    valid = gtk_tree_model_get_iter_first (model, &iter);
    while(valid)
    {
        UIngredient *ing;
        gtk_tree_model_get(model, &iter,
                            ING_COLUMN_OBJECT, &ing,
                           -1); // Terminate
        if(!ing)
            g_log(ULTRA_LOG,
                  G_LOG_LEVEL_WARNING,
                  "Ingredient is null!");
        else
            dbg_print_ingredient(ing);
        g_object_unref(ing);
        valid = gtk_tree_model_iter_next (model, &iter);
    }
}

void
dbg_print_recipes()
{
    g_log(ULTRA_LOG,
          G_LOG_LEVEL_INFO,
          "Start dbg print of recipes:");
    GtkTreeModel *model = GTK_TREE_MODEL(recListStore);
    GtkTreeIter iter;
    gboolean valid;

    valid = gtk_tree_model_get_iter_first (model, &iter);
    while(valid)
    {
        URecipe *rec;
        gtk_tree_model_get(model, &iter,
                           ING_COLUMN_OBJECT, &rec,
                           -1); // Terminate
        if(!rec)
            g_log(ULTRA_LOG,
                  G_LOG_LEVEL_WARNING,
                  "Recipe is null!");
        else
            dbg_print_recipe(rec);
        g_object_unref(rec);
        valid = gtk_tree_model_iter_next (model, &iter);
    }
}
#endif // ULTRA_DEBUG