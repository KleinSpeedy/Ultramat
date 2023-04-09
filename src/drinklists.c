/* This file implements creating GTK ListStore objects for recipes and ingredients
 * from their respective arrays */

#include "drinklists.h"
#include "checks.h"

/* function implementation */

/**
 * @brief Creates GTK ListStore from ingredients array
 * 
 * @return ListStore with ingredients
*/
GtkListStore *create_ingredient_listStore(Ing_Array_t *ingArray)
{
    GtkListStore *ingListStore = GTK_LIST_STORE(gtk_list_store_new(
        ING_NUM_COLUMN,
        G_TYPE_STRING,
        G_TYPE_INT,
        G_TYPE_INT,
        G_TYPE_BOOLEAN));
    CHECK_WIDGET(ingListStore, "ingredients list store");

    GtkTreeIter iterator;

    // copy data from ing array to list store
    for(gint i = 0; i < ingArray->size; i++)
    {
        Ingredient_t current = ingredients_get_at(ingArray, i);

        gchar *gname = current.name;
        guint8 gid = current.id;
        guint8 gposition = current.position;
        gboolean gselected = current.selected; // todo: check conversion

        // new row
        gtk_list_store_append(ingListStore, &iterator);
        gtk_list_store_set(ingListStore, &iterator,
            ING_COLUMN_NAME, gname,
            ING_COLUMN_ID, gid,
            ING_COLUMN_POSITION, gposition,
            ING_COLUMN_SELECTED, gselected,
            -1); // terminate
    }

    return ingListStore;
}

/**
 * @brief creates GTK ListStore from recipes array
 * 
 * @return ListStore with recipes
*/
GtkListStore *create_recipe_listStore(Rec_Array_t *recipeArray)
{
    GtkListStore *recipeListStore = gtk_list_store_new(
        REC_NUM_COLUMNS,
        G_TYPE_STRING,
        G_TYPE_INT,
        G_TYPE_BOOLEAN);
    CHECK_WIDGET(recipeListStore, "Recipe List Store");

    GtkTreeIter iterator;

    for(int i = 0; i < recipeArray->size; i++)
    {
        Recipe_t current = recipe_get_at(recipeArray, i);

        gchar *gname = current.name;
        guint8 gid = current.id;
        gboolean gavailable = current.available;

        gtk_list_store_append(recipeListStore, &iterator);
        gtk_list_store_set(recipeListStore, &iterator,
            REC_COLUMN_NAME, gname,
            REC_COLUMN_ID, gid,
            REC_COLUMN_AVAILABLE, gavailable,
            -1); // terminate 
    }

    return recipeListStore;
}
