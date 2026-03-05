#include "drinks_db.h"
#include "gtk/gtk.h"
#include <assert.h>
#include <sqlite3.h>
#include <stdio.h>

typedef enum
{
    ING_TYPE_SOFTDRINK = 0,
    ING_TYPE_JUICE,
    ING_TYPE_LIQUOR,

    ING_TYPE_MAX
} ing_type_t;

#define DB_NAME "bar_mixer.db"
#define MAX_ING_POSITIONS 9

// IDs on position represent a selected ingredient
static int ingPositions_[MAX_ING_POSITIONS] = {0};

/* "private" functions */

// Helper function to get the absolute path to a resource
static const char *_get_resource_path(const char *filename)
{
    if(!filename)
    {
        return NULL;
    }

    const char *base_dir = g_getenv("ULTRAMAT_RES_DIR");

    if(base_dir != NULL)
    {
        return g_build_filename(base_dir, filename, NULL);
    }
    else
    {
        return g_build_filename("res", filename, NULL);
    }
}

static GdkPixbuf *_load_icon(const char *filename)
{
    GError *error = NULL;
    GdkPixbuf *pixbuf;
    GtkIconTheme *theme = gtk_icon_theme_get_default();

    if(!filename)
    {
        return gtk_icon_theme_load_icon(theme, "image-missing", 128, 0, NULL);
    }
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

static gboolean _ingredient_is_selected(const int ing_id)
{
    for(int i = 0; i < MAX_ING_POSITIONS; i++)
    {
        if(ingPositions_[i] == ing_id)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static int _check_recipe(sqlite3 *db, const int recipe_id)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT i.id "
                      "FROM ingredients i "
                      "JOIN recipe_steps rs ON i.id = rs.ingredient_id "
                      "WHERE rs.recipe_id = ?";

    int rc = sqlite3_prepare(db, sql, -1, &stmt, NULL);
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "Fehler beim Vorbereiten: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite3_bind_int(stmt, 1, recipe_id);

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const int ing_id = sqlite3_column_int(stmt, 0);

        if(!_ingredient_is_selected(ing_id))
        {
            sqlite3_finalize(stmt);
            return -1;
        }
    }

    if(rc != SQLITE_DONE)
    {
        fprintf(stderr, "Fehler beim Auslesen: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_finalize(stmt);
    return 1;
}

static int _fill_ing_list_store(sqlite3 *db, GtkListStore *ls, const char *sql,
                                const ing_type_t type)
{
    sqlite3_stmt *stmt;

    int result = sqlite3_prepare(db, sql, -1, &stmt, NULL);
    if(result != SQLITE_OK)
    {
        fprintf(stderr, "Fehler beim Vorbereiten: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite3_bind_int(stmt, 1, type);

    while((result = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const int ing_id = sqlite3_column_int(stmt, 0);
        const unsigned char *ing_name = sqlite3_column_text(stmt, 1);
        const unsigned char *icon_name = sqlite3_column_text(stmt, 2);

        const gchar *name = g_strdup((const char *)ing_name);
        const guint32 id = ing_id;

        // construct sub dir image path to load icon as pixbuf
        char img_path[128] = {0};
        snprintf(img_path, sizeof(img_path), "img/%s", icon_name);
        const char *full_path = _get_resource_path(img_path);
        GdkPixbuf *icon = _load_icon(full_path);

        GtkTreeIter iter;
        gtk_list_store_append(ls, &iter);
        gtk_list_store_set(ls, &iter, ING_COLUMN_NAME, name, ING_COLUMN_ID, id,
                           ING_COLUMN_ICON, icon,
                           -1); // terminate

        printf("%d - %s %s\n", ing_id, name, full_path);

        // free stuff
        g_free((gpointer)full_path);
        g_free((gpointer)name);
        g_object_unref(icon);
    }

    if(result != SQLITE_DONE)
    {
        fprintf(stderr, "Fehler beim Auslesen: %s\n", sqlite3_errmsg(db));
        result = -1;
    }

    sqlite3_finalize(stmt);
    return result;
}

static int _fill_rec_list_store(sqlite3 *db, GtkListStore *ls, const char *sql)
{
    (void)ls;
    sqlite3_stmt *stmt;

    int result = sqlite3_prepare(db, sql, -1, &stmt, NULL);
    if(result != SQLITE_OK)
    {
        fprintf(stderr, "Fehler beim Vorbereiten: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    while((result = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const int rec_id = sqlite3_column_int(stmt, 0);
        const unsigned char *rec_name = sqlite3_column_text(stmt, 1);

        const gchar *name = g_strdup((const char *)rec_name);
        const guint32 id = rec_id;

        GtkTreeIter iter;
        gtk_list_store_append(ls, &iter);
        gtk_list_store_set(ls, &iter, REC_COLUMN_NAME, name, REC_COLUMN_ID, id,
                           -1); // terminate

        printf("%d - %s\n", rec_id, name);

        g_free((gpointer)name);
    }

    if(result != SQLITE_DONE)
    {
        fprintf(stderr, "Fehler beim Auslesen: %s\n", sqlite3_errmsg(db));
        result = -1;
    }

    sqlite3_finalize(stmt);
    return result;
}

/* "public" functions */

int drinks_check_recipe(const int recipe_id)
{
    sqlite3 *db;
    if(sqlite3_open(DB_NAME, &db) != SQLITE_OK)
    {
        fprintf(stderr, "Kann DB nicht öffnen: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    const int result = _check_recipe(db, recipe_id);
    printf("Check recipe %d - result: %d\n", recipe_id, result);
    sqlite3_close(db);
    return result;
}

int drinks_set_ing_pos(const int ing_id, const int pos)
{
    assert(pos >= 0 && pos < MAX_ING_POSITIONS);

    printf("before: %d\n", ingPositions_[pos]);

    // check if ingredient is already selected elsewhere
    if(_ingredient_is_selected(ing_id))
        return -1;

    ingPositions_[pos] = ing_id;
    return 1;
}

void drinks_reset_ing_pos(const int pos)
{
    assert(pos >= 0 && pos < MAX_ING_POSITIONS);
    ingPositions_[pos] = -1;
}

int drinks_ing_liquor_list_store(GtkListStore *ls)
{
    sqlite3 *db;
    if(sqlite3_open(DB_NAME, &db) != SQLITE_OK)
    {
        fprintf(stderr, "Kann DB nicht öffnen: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    const char *sql = "SELECT i.id, i.name, i.icon_name "
                      "FROM ingredients i "
                      "WHERE i.type = ?";

    const int result = _fill_ing_list_store(db, ls, sql, ING_TYPE_LIQUOR);
    sqlite3_close(db);
    return result;
}

int drinks_ing_pump_list_store(GtkListStore *ls)
{
    sqlite3 *db;
    if(sqlite3_open(DB_NAME, &db) != SQLITE_OK)
    {
        fprintf(stderr, "Kann DB nicht öffnen: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    const char *sql = "SELECT i.id, i.name, i.icon_name "
                      "FROM ingredients i "
                      "WHERE i.type != ?";

    const int result = _fill_ing_list_store(db, ls, sql, ING_TYPE_LIQUOR);
    sqlite3_close(db);
    return result;
}

int drinks_rec_list_store(GtkListStore *ls)
{
    sqlite3 *db;
    if(sqlite3_open(DB_NAME, &db) != SQLITE_OK)
    {
        fprintf(stderr, "Kann DB nicht öffnen: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    const char *sql = "SELECT * FROM recipes";

    const int result = _fill_rec_list_store(db, ls, sql);
    sqlite3_close(db);
    return result;
}
