#include "inc/drinklists.h"
#include "inc/drinks.h"

/* private functions */

/* function implementation */

GtkListStore *createIngredientListStore(Ing_Array_t *ingArray)
{
    // ingredient informations for list store 
    enum {
        COLUMN_NAME,
        COLUMN_ID,
        COLUMN_POSITION,
        COLUMN_SELECTED,

        NUM_COLUMN
    };
    
    GtkListStore *ingListStore = GTK_LIST_STORE(gtk_list_store_new(
        NUM_COLUMN,
        G_TYPE_STRING,
        G_TYPE_INT,
        G_TYPE_INT,
        G_TYPE_BOOLEAN));
    CHECK_WIDGET(ingListStore, "ingredients list store");

    GtkTreeIter iterator;

    // copy data from ing array to list store
    for(gint i = 0; i < ingArray->size; i++)
    {
        Ingredient_t current = get_at_ing_array(ingArray, i);

        gchar *gname = current.name;
        guint8 gid = current.id;
        guint8 gposition = current.position;
        gboolean gselected = current.selected; // todo: check conversion

        // new row
        gtk_list_store_append(ingListStore, &iterator);
        gtk_list_store_set(ingListStore, &iterator,
            COLUMN_NAME, gname,
            COLUMN_ID, gid,
            COLUMN_POSITION, gposition,
            COLUMN_SELECTED, gselected,
            -1); // terminate
    }

    return ingListStore;
}

GtkListStore *createRecipeListStore(Rec_Array_t *recipeArray)
{
    enum {
        COLUMN_NAME,
        COLUMN_ID,
        COLUMN_AVAILABLE,

        NUM_COLUMNS
    };

    GtkListStore *recipeListStore = gtk_list_store_new(
        NUM_COLUMNS,
        G_TYPE_STRING,
        G_TYPE_INT,
        G_TYPE_BOOLEAN);
    CHECK_WIDGET(recipeListStore, "Recipe List Store");

    GtkTreeIter iterator;

    for(int i = 0; i < recipeArray->size; i++)
    {
        Recipe_t current = get_at_rec_array(recipeArray, i);

        gchar *gname = current.name;
        guint8 gid = current.id;
        gboolean gavailable = current.available;

        gtk_list_store_append(recipeListStore, &iterator);
        gtk_list_store_set(recipeListStore, &iterator,
            COLUMN_NAME, gname,
            COLUMN_ID, gid,
            COLUMN_AVAILABLE, gavailable,
            -1); // terminate 
    }

    return recipeListStore;
}