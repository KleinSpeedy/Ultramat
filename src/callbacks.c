#include "callbacks.h"

void on_combo_pos1_changed(GtkComboBox *comboBox, gpointer data)
{
    GtkTreeModel *list_store;
    GtkTreeIter iter;
    gboolean valid;
    gint row_count = 0;
    
    list_store = gtk_combo_box_get_model(comboBox);

    // Get the first iter in the list, check it is valid and walk
    // through the list, reading each row.

    valid = gtk_tree_model_get_iter_first(list_store,
                                        &iter);
    while (valid)
    {
    gchar *str_data;

    // Make sure you terminate calls to `gtk_tree_model_get()` with a “-1” value
    gtk_tree_model_get(list_store, &iter,
                        0, &str_data,
                        -1);

    // Do something with the data
    g_print ("Row %d: (%s)\n",
                row_count, str_data);
    g_free (str_data);

    valid = gtk_tree_model_iter_next (list_store,
                                        &iter);
    row_count++;
    }
}

void on_combo_pos2_changed()
{

}