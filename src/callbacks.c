#include "callbacks.h"
#include "drinkarray.h"
#include "drinklists.h"
#include <stdint.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

extern GtkWindow *g_mainWindow;
#define REPLACE GTK_RESPONSE_ACCEPT
#define DISCARD GTK_RESPONSE_REJECT

// Callback pressing replace
static void cb_dialog_replace(GtkWidget *dialog)
{
    // Somehow seems to work
    gtk_window_close(GTK_WINDOW(dialog));
}

// Callback pressing discard
static void cb_dialog_discard(GtkDialog *dialog)
{
    // Somehow seems to work
    gtk_window_close(GTK_WINDOW(dialog));
}

// popup message if row already selected
static gint on_already_selected(gchar *message, gint position)
{
    GtkDialogFlags flags;
    GtkWidget *label, *content_area, *dialog;
    GtkWidget *buttonReplace, *buttonDiscard;

    flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
    dialog = gtk_dialog_new();

    buttonReplace = gtk_dialog_add_button(GTK_DIALOG(dialog), "Ersetzen", REPLACE);
    buttonDiscard = gtk_dialog_add_button(GTK_DIALOG(dialog), "Verwerfen", DISCARD);

    // Callbacks
    g_signal_connect(buttonReplace, "clicked", G_CALLBACK(cb_dialog_replace), dialog);
    g_signal_connect(buttonDiscard, "clicked", G_CALLBACK(cb_dialog_discard), dialog);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Create new modal message with Prefix Body and current position
    char *messageBody = " is already selected at ";
    // TODO: Make dynamic alloc
    char buffer[100];
    snprintf(buffer, sizeof(buffer),"%s%s%d", message, messageBody, position);

    label = gtk_label_new(buffer);

    // Add the label, and show everything we’ve added
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);

    // TODO: find solution for creating dialog with ACCEPT/REJECT
    return gtk_dialog_run(GTK_DIALOG(dialog));
}

// Show error msg if something goes wrong
static void show_error_msg(gchar *errorStr)
{
    GtkWidget *dialog;

    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new(g_mainWindow,
            flags,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_CLOSE,
            "%s\n",
            errorStr);

    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}


static void apply_modal_decision(gint decision, GtkComboBox *comboBox)
{
    switch(decision)
    {
        // TODO: Delete entry on previous comboBox
        case REPLACE:
        {
            g_print("Replace\n");
            break;
        }
        // Delete item in current comboBox
        case DISCARD:
        {
            g_print("Discard\n");
            gtk_combo_box_set_active(comboBox, -1);
            break;
        }
        default:
        {
            show_error_msg("Error applying decision\n");
            break;
        }
    }
}

// callbacks

// On changed signal get selected row and check whether entry is unique or not
// update entry on success or open simple modal dialog if not unique
void on_combo_pos_changed(GtkComboBox *comboBox, gpointer data)
{
    GtkTreeModel *listStore;
    GtkTreeIter iter;
    gboolean selected;
    gchararray name;
    guint rowPosition, cbPosition;

    cbPosition = GPOINTER_TO_INT(data);

    listStore = gtk_combo_box_get_model(comboBox);

    if(!gtk_combo_box_get_active_iter(comboBox,&iter))
    {
        show_error_msg("Couldnt fetch ingredients data!");
        return;
    }

    // fetch row data
    gtk_tree_model_get(listStore, &iter,
            ING_COLUMN_NAME, &name,
            ING_COLUMN_SELECTED, &selected,
            ING_COLUMN_POSITION, &rowPosition,
            -1);

    if(selected)
    {
        // ingredient selected elsewhere
        if(cbPosition != rowPosition)
        {
            gint decision = on_already_selected(name, rowPosition);
            apply_modal_decision(decision, comboBox);
        }
        else
        {
            // This shouldn't happen as reselecting shouldn't be possible
            show_error_msg("Reselect Error!");
        }
    }
    else
    {
       //  Set position and selected
       gtk_list_store_set(GTK_LIST_STORE(listStore), &iter,
               ING_COLUMN_POSITION, cbPosition,
               ING_COLUMN_SELECTED, TRUE,
               -1);
    }

    g_print("%s %d %d\n", name, selected, rowPosition);
    // Free necessary?
    g_free(name);
}

