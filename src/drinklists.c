/*
 * This file implements creating GTK ListStore objects for recipes and
 * ingredients from their respective arrays
 */

#include "drinklists.h"
#include "checks.h"
#include "drinks.h"

#include <stdio.h>
#include <gtk/gtk.h>

// local ingredient list store holds all ingredient objects
static GtkListStore *ingListStore = NULL;
// local recipe list store holds all recipe objects
static GtkListStore *recListStore = NULL;

static void lists_add_ingredients(VLArray_t *ingArray)
{
    for(uint32_t i = 0; i < ingArray->used; ++i)
    {
        Ingredient *ing = vla_get_elem_at(ingArray, i);

        //Ingredient *ing = ingArray->get(ingArray, i);
        const gchar *name = g_strdup(ing->name);
        const guint32 id = ing->id;

        GtkTreeIter iter;
        gtk_list_store_append(ingListStore, &iter);
        gtk_list_store_set(ingListStore, &iter,
                            ING_COLUMN_NAME, name,
                            ING_COLUMN_ID, id,
                            -1); // terminate
    }
}

static void lists_add_recipes(VLArray_t *recArray)
{
    for(uint32_t i = 0; i < recArray->used; ++i)
    {
        Recipe *rec = vla_get_elem_at(recArray, i);

        const gchar *name = g_strdup(rec->name);
        const guint32 id = rec->id;

        GtkTreeIter iter;
        gtk_list_store_append(recListStore, &iter);
        gtk_list_store_set(recListStore, &iter,
                            REC_COLUMN_NAME, name,
                            REC_COLUMN_ID, id,
                            -1); // terminate
    }
}

/* function implementation */

void *lists_ingredient_store(void)
{
    return ingListStore;
}

void *lists_recipe_store(void)
{
    return recListStore;
}

void lists_create_ingredient_store(VLArray_t *ingArray)
{
    ingListStore = gtk_list_store_new(
            ING_NUM_COLUMN,
            G_TYPE_STRING,
            G_TYPE_UINT);
    CHECK_WIDGET(ingListStore, "ingredients list store");
    lists_add_ingredients(ingArray);
}

void lists_create_recipe_store(VLArray_t *recArray)
{
    recListStore = gtk_list_store_new(
            REC_NUM_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_UINT);
    CHECK_WIDGET(recListStore, "Recipe List Store");
    lists_add_recipes(recArray);
}
