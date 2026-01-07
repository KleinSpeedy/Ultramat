/*
 * This file implements creating GTK ListStore objects for recipes and
 * ingredients from their respective arrays
 */

#include "drinklists.h"
#include "checks.h"
#include "drinks.h"

#include <gtk/gtk.h>
#include <stdio.h>

// local ingredient list store holds all ingredient objects
static GtkListStore *ingListStore = NULL;
// local recipe list store holds all recipe objects
static GtkListStore *recListStore = NULL;

// NOTE: This list has to have the same order as ingredients.txt
static const char *const ICON_PATHS[] = {
    "res/img/cmp_osaft.png", "res/img/cmp_asaft.png", NULL,
    "res/img/cmp_rum.png",   "res/img/cmp_gin.png",   "res/img/cmp_vodka.png",
    "res/img/cmp_tonic.png", "res/img/cmp_soda.png"};

static GdkPixbuf *load_icon(const char *filename)
{
    GError *error = NULL;
    GdkPixbuf *pixbuf;
    GtkIconTheme *theme = gtk_icon_theme_get_default();

    if(!filename)
    {
        g_info("No image needed\n");
        return gtk_icon_theme_load_icon(theme, "image-missing", 128, 0, NULL);
    }
    // WICHTIG: _at_scale nutzen, sonst sprengen große Bilder das Layout!
    pixbuf =
        gdk_pixbuf_new_from_file_at_scale(filename, 128, 128, TRUE, &error);

    if(!pixbuf)
    {
        g_warning("Bild konnte nicht geladen werden: %s", error->message);
        g_error_free(error);
        return gtk_icon_theme_load_icon(theme, "image-missing", 128, 0, NULL);
    }
    return pixbuf;
}

static void lists_add_ingredients(VLArray_t *ingArray)
{
    for(uint32_t i = 0; i < ingArray->used; ++i)
    {
        Ingredient *ing = vla_get_elem_at(ingArray, i);

        const gchar *name = g_strdup(ing->name);
        const guint32 id = ing->id;
        GdkPixbuf *icon = load_icon(ICON_PATHS[i]);

        GtkTreeIter iter;
        gtk_list_store_append(ingListStore, &iter);
        gtk_list_store_set(ingListStore, &iter, ING_COLUMN_NAME, name,
                           ING_COLUMN_ID, id, ING_COLUMN_ICON, icon,
                           -1); // terminate

        g_free((gpointer)name);
        g_object_unref(icon);
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
        gtk_list_store_set(recListStore, &iter, REC_COLUMN_NAME, name,
                           REC_COLUMN_ID, id,
                           -1); // terminate

        g_free((gpointer)name);
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
    ingListStore = gtk_list_store_new(ING_NUM_COLUMN, G_TYPE_STRING,
                                      G_TYPE_UINT, G_TYPE_OBJECT);
    CHECK_WIDGET(ingListStore, "ingredients list store");
    lists_add_ingredients(ingArray);
}

void lists_create_recipe_store(VLArray_t *recArray)
{
    recListStore =
        gtk_list_store_new(REC_NUM_COLUMNS, G_TYPE_STRING, G_TYPE_UINT);
    CHECK_WIDGET(recListStore, "Recipe List Store");
    lists_add_recipes(recArray);
}
