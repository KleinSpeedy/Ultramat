#include "callbacks.h"
#include "drinkarray.h"
#include "drinklists.h"
#include "serialcom.h"
#include <stdint.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get access to main Window
extern GtkWindow *g_mainWindow;

#define REPLACE GTK_RESPONSE_ACCEPT
#define DISCARD GTK_RESPONSE_REJECT

// Replace current text with nothing
#define RESET_TEXT "Zutat auswählen!"

// error str buffer size
#define ERR_STR_BUFFER 100

// HACK?: make Handler IDs global
// 6 IDs for 6 callbacks
// TODO: make 6 a #define
gulong g_handlerIds[6];

// handler id for orderStart toggle button
gulong g_handlerIdOrderStart;

// handler id for motor switch
gulong g_handlerIdMotorSwitch;

// keep track of last items selected
static GtkTreeIter *positionIters[6];

static gboolean MOTORS_ENABLED = FALSE;

/* PRIVATE FUNCTIONS */

// Show error msg if something goes wrong
static void
show_error_msg(gchar *errorStr)
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

// 
static void
cb_decision_replace()
{
}

// unset item of current combo box and reset text in GtkEntry
static void
cb_decision_discard(GtkComboBox *comboBox,
        GtkTreeIter *activeIter,
        gulong handlerId)
{
    GtkTreeModel *tempModel = gtk_combo_box_get_model(comboBox);

    // block signal emit so we are not resetting comboBox Entry
    g_signal_handler_block(comboBox, handlerId);

    // Reset current entry text
    gtk_combo_box_set_active_iter(comboBox, activeIter);
    GtkEntry *comboEntry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboBox)));
    gtk_entry_set_text(comboEntry, RESET_TEXT);

    g_signal_handler_unblock(comboBox, handlerId);
}

// apply replace or discard decision from modal dialog
static void
apply_modal_decision(gint decision,
        GtkComboBox *comboBox,
        GtkTreeIter *activeIter,
        gulong handlerId,
        guint position)
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
            cb_decision_discard(comboBox, activeIter, handlerId);
            break;
        }
        default:
        {
            show_error_msg("Unkonw decision value!");
            break;
        }
    }
}

// popup message if row already selected
static gint
on_already_selected(gchar *name, gint position)
{
    GtkWidget *label, *content_area, *dialog;
    GtkWidget *buttonReplace, *buttonDiscard;

    dialog = gtk_dialog_new();
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    buttonReplace = gtk_dialog_add_button(GTK_DIALOG(dialog), "Ersetzen", REPLACE);
    buttonDiscard = gtk_dialog_add_button(GTK_DIALOG(dialog), "Verwerfen", DISCARD);

    // Callbacks
    g_signal_connect(buttonReplace, "clicked", G_CALLBACK(gtk_window_close), dialog);
    g_signal_connect(buttonDiscard, "clicked", G_CALLBACK(gtk_window_close), dialog);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    char buffer[ERR_STR_BUFFER];

    memset(buffer, 0, ERR_STR_BUFFER);
    snprintf(buffer, ERR_STR_BUFFER, "%s is already selected at %d", name, position);

    label = gtk_label_new(buffer);

    // Add the label, and show everything we’ve added
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);

    return gtk_dialog_run(GTK_DIALOG(dialog));
}

// Change order toggle button status back to inactive
// without emitting signal
static void
cb_reset_toggle_status(GtkToggleButton *toggleButton)
{
    g_signal_handler_block(toggleButton, g_handlerIdOrderStart);
    gtk_toggle_button_set_active(toggleButton, FALSE);
    g_signal_handler_unblock(toggleButton, g_handlerIdOrderStart);
}

// Set status without emitting signal
static void
cb_set_switch_state(GtkSwitch *sw, gboolean newState)
{
    g_signal_handler_block(sw, g_handlerIdMotorSwitch);
    gtk_switch_set_state(sw, newState);
    gtk_switch_set_active(sw, newState);
    g_signal_handler_unblock(sw, g_handlerIdMotorSwitch);
}

/* CALLBACK IMPLEMENTATIONS */

// On changed signal get selected row and check whether entry is unique or not
// update entry on success or open simple modal dialog if not unique
void on_combo_pos_changed(GtkComboBox *comboBox, gpointer data)
{
    GtkTreeModel *comboModel;
    GtkTreeIter tempIter;

    gboolean selected;
    gchararray name;

    // row in liststore and combo position
    guint rowPosition, cbPosition;
    gulong currentHandlerId = 0;

    cbPosition = GPOINTER_TO_INT(data);

    // get current callback handler ID
    if(cbPosition >= 0 && cbPosition < 6)
    {
        currentHandlerId = g_handlerIds[cbPosition];
    }
    else
    {
        show_error_msg("Error calculating callback position!");
        return;
    }

    comboModel = gtk_combo_box_get_model(comboBox);

    // check if position was already used and if so clear it
    if(positionIters[cbPosition])
    {
        gtk_list_store_set(GTK_LIST_STORE(comboModel), positionIters[cbPosition],
                ING_COLUMN_SELECTED, FALSE,
                -1); // end
        gtk_tree_iter_free(positionIters[cbPosition]);
    }

    if(!gtk_combo_box_get_active_iter(comboBox, &tempIter))
    {
        show_error_msg("Couldnt fetch ingredients data!");
        return;
    }

    // fetch row data
    gtk_tree_model_get(comboModel, &tempIter,
            ING_COLUMN_NAME, &name,
            ING_COLUMN_SELECTED, &selected,
            ING_COLUMN_POSITION, &rowPosition,
            -1); // end

    // item was already selected once
    // TODO: Check necessary?
    if(selected)
    {
        // ingredient selected elsewhere
        if(cbPosition != rowPosition)
        {
            gint decision = on_already_selected(name, rowPosition+1);
            apply_modal_decision(decision,
                    comboBox, &tempIter,
                    currentHandlerId, cbPosition);
            return;
        }
        else
        {
            // This shouldn't happen as reselecting shouldn't be possible
            show_error_msg("Reselect Error!");
            return;
        }
    }

    //  Set position and selected
    gtk_list_store_set(GTK_LIST_STORE(comboModel), &tempIter,
            ING_COLUMN_POSITION, cbPosition,
            ING_COLUMN_SELECTED, TRUE,
            -1);

    g_print("%s %d %d\n", name, selected, rowPosition);
    positionIters[cbPosition] = gtk_tree_iter_copy(&tempIter);

    // Free necessary?
    g_free(name);
}

gboolean
on_motor_switch_toggle(GtkSwitch *motorSwitch, gboolean state, gpointer data)
{
    int ret;
    MOTORS_ENABLED = state;

    if(MOTORS_ENABLED)
    {
        ret = serialcom_initialize_connection();
        if(!ret)
        {
            cb_set_switch_state(motorSwitch, state);
        }
        else
        {
            cb_set_switch_state(motorSwitch, FALSE);
            show_error_msg("Unable to open serial connection\n"
                    "Check USB cable connection!");
        }
    }
    else
    {
        ret = serialcom_cancel_connection();
        if(!ret)
        {
            cb_set_switch_state(motorSwitch, state);
        }
        else
        {
            // TODO: Which state to set?
            show_error_msg("Unable to close serial connection!");
        }
    }
    // prevent default handler from running
    return TRUE;
}

void
on_drink_order_toggle(GtkToggleButton *orderButton, gpointer data)
{
    GtkTreeIter activeIter;

    if(MOTORS_ENABLED)
    {

    }
    else
    {
        show_error_msg("Motors arent activated, start Serial first!");
        cb_reset_toggle_status(orderButton);
        return;
    }
    if(data)
    {
        GtkListStore *ingListStore = data;
        gboolean valid, selected;
        gchar *drink;

        valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(ingListStore),
                &activeIter);

        while(valid)
        {
            gtk_tree_model_get(GTK_TREE_MODEL(ingListStore), &activeIter,
                    ING_COLUMN_NAME, &drink,
                    ING_COLUMN_SELECTED, &selected,
                    -1);
            g_print("%s selected: %d \n", drink, selected);

            valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(ingListStore),
                        &activeIter);
        }
        cb_reset_toggle_status(orderButton);
    }
    else
    {
        show_error_msg("Error fetching callback data!");
    }
}
