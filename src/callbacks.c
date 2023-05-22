/*
 * Implements GTK Widget callbacks and neccessary "helper" functions.
 */

#include "callbacks.h"
#include "drinklists.h"
#include "serialcom.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>

#define REPLACE GTK_RESPONSE_ACCEPT
#define DISCARD GTK_RESPONSE_REJECT

// Replace current text with default text
#define RESET_TEXT "Zutat auswählen!"

// error str buffer size
#define ERR_STR_BUFFER 100

// get access to main Window
extern GtkWindow *g_mainWindow;

// HACK?: make Handler IDs global
// 6 IDs for 6 callbacks
gulong g_handlerIds[6];

// handler id for orderStart toggle button
gulong g_handlerIdOrderStart;

// handler id for motor switch
gulong g_handlerIdMotorSwitch;

// handler id for recipe order combo box
gulong g_handlerIdComboOrder;

// keep track of last items selected, one Iter per comboBox
static GtkTreeIter *positionIters[6];
// keep track of selected recipe
static GtkTreeIter recipeIter;

static gboolean MOTORS_ENABLED = FALSE;

/* ========== STATIC FUNCTION DECLARATION ========== */

static void show_error_msg(gchar *errorStr);

static void cb_decision_replace();

static void cb_decision_discard(GtkComboBox *comboBox, gulong handlerId);

static void apply_modal_decision(
        gint decision,
        GtkComboBox *comboBox,
        gulong handlerId,
        guint position);

static gint on_already_selected(const gchar *name, guint position);

static void cb_reset_toggle_status(GtkToggleButton *toggleButton);

static gboolean cb_check_recipe();

/* IMPLEMENTATION */

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
cb_decision_discard(GtkComboBox *comboBox, gulong handlerId)
{
    // block signal emit because we are setting active entry
    g_signal_handler_block(comboBox, handlerId);

    // Reset current entry text
    gtk_combo_box_set_active_iter(comboBox, NULL);
    GtkEntry *comboEntry = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboBox)));
    gtk_entry_set_text(comboEntry, RESET_TEXT);

    g_signal_handler_unblock(comboBox, handlerId);
}

// apply replace or discard decision from modal dialog
static void
apply_modal_decision(gint decision,
        GtkComboBox *comboBox,
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
            cb_decision_discard(comboBox, handlerId);
            break;
        }
        default:
        {
            show_error_msg("Unknown decision value!");
            break;
        }
    }
}

// popup dialog window if row is already selected
static gint
on_already_selected(const gchar *name, guint position)
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
    // return modal dialog decision
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
    MOTORS_ENABLED = newState;
    gtk_switch_set_state(sw, newState);
    gtk_switch_set_active(sw, newState);
    g_signal_handler_unblock(sw, g_handlerIdMotorSwitch);
}

// check if all needed ingredients are currently selected
static gboolean
cb_check_recipe()
{
    gboolean available = FALSE;
    GtkListStore *recStore = lists_recipe_store();
    URecipe *selected;

    // TODO: function is slow, find different solution!
    // -> Pass comboModel into this function and get active iter, no need for recipeIter!
    if(!(gtk_list_store_iter_is_valid(recStore, &recipeIter)))
    {
        show_error_msg("Recipe entry is invalid!");
        return FALSE;
    }

    gtk_tree_model_get(GTK_TREE_MODEL(recStore), &recipeIter,
            REC_COLUMN_OBJECT, &selected,
            -1); // terminate
    // check if recipe is available
    available = u_recipe_is_available(selected);
    g_object_unref(selected);

    return available;
}

/* ========== CALLBACK IMPLEMENTATIONS ========== */

// gets called when selecting a new ingredient and needs to handle reselection
// on a different position as well as discarding current selection
/**
 * @brief Callback for selecting a new Ingredient on a ComboBox
 * This callback needs to handle a possible reselection too
 *
 * @param comboBox callback ComboBox
 * @param data custom data - position of Callback as pointer
 */
void
on_combo_pos_changed(GtkComboBox *comboBox, gpointer data)
{

    GtkTreeModel *comboModel;
    GtkTreeIter activeIter;
    UIngredient *selectedIng;

    // row in liststore and combo position
    guint ingPosition, cbPosition;
    gulong currentHandlerId = 0;

    cbPosition = GPOINTER_TO_INT(data);

    // get current callback handler ID
    if(cbPosition < 6)
    {
        currentHandlerId = g_handlerIds[cbPosition];
    }
    else
    {
        show_error_msg("Error calculating callback position!");
        return;
    }

    comboModel = gtk_combo_box_get_model(comboBox);

    // TODO: Whats this?
    // check if position was already used and if so clear it
    if(positionIters[cbPosition])
    {
        /*
        gtk_list_store_set(GTK_LIST_STORE(comboModel), positionIters[cbPosition],
                ING_COLUMN_SELECTED, FALSE,
                -1); // end
        gtk_tree_iter_free(positionIters[cbPosition]);
        */
    }

    if(!gtk_combo_box_get_active_iter(comboBox, &activeIter))
    {
        show_error_msg("Couldnt fetch ingredients data!");
        return;
    }

    // fetch row data
    gtk_tree_model_get(comboModel, &activeIter,
        ING_COLUMN_OBJECT, &selectedIng,
            -1); // end

    ingPosition = u_ingredient_get_position(selectedIng);
    // item was already selected once
    // TODO: Check necessary?
    if(u_ingredient_get_selected(selectedIng))
    {
        // ingredient selected elsewhere
        if(cbPosition != ingPosition)
        {
            // TODO: make this one function
            gint decision = on_already_selected(u_ingredient_get_name(selectedIng),
                                                (gint)ingPosition + 1);
            apply_modal_decision(decision, comboBox,currentHandlerId, cbPosition);
            return;
        }
        else
        {
            // This shouldn't happen as "reselecting" shouldn't be possible
            show_error_msg("Reselect Error!");
            return;
        }
    }

    u_ingredient_set_selected(selectedIng, TRUE);
    u_ingredient_set_position(selectedIng, cbPosition);
    //  Set position and selected
    gtk_list_store_set(GTK_LIST_STORE(comboModel), &activeIter,
            ING_COLUMN_OBJECT, &selectedIng,
            -1);

    g_print("%s %d %d\n",
            u_ingredient_get_name(selectedIng),
            u_ingredient_get_selected(selectedIng),
            ingPosition);
    positionIters[cbPosition] = gtk_tree_iter_copy(&activeIter);
    g_object_unref(selectedIng);
}

gboolean
on_motor_switch_toggle(GtkSwitch *motorSwitch, gboolean state, gpointer data)
{
    // get out if everything is TRUE
    if(MOTORS_ENABLED && state)
        return TRUE;

    // new activation
    if(!MOTORS_ENABLED)
    {
        if(serialcom_initialize_connection() != 0)
        {
            cb_set_switch_state(motorSwitch, FALSE);
            show_error_msg("Unable to open serial connection\n"
                    "Check USB cable connection!");
        }
        else
        {
            cb_set_switch_state(motorSwitch, state);
        }
    }
    // new stop signal
    else
    {
        if(serialcom_cancel_connection() != 0)
        {
            cb_set_switch_state(motorSwitch, FALSE);
            show_error_msg("Unable to close serial connection!");
            return FALSE;
        }
        else
        {
            cb_set_switch_state(motorSwitch, state);
        }
    }
    // prevents default handler from running
    return TRUE;
}

// sets currently selected recipe to active item
void
on_combo_order_changed(GtkComboBox *comboBox, gpointer data)
{
    (void)data;
    // get model and active item
    gtk_combo_box_get_active_iter(comboBox, &recipeIter);
}

// gets called when starting a new mix and checks if all ingredients 
// for the recipe are available, if so it needs to send the serial commands
void
on_recipe_order_toggle(GtkToggleButton *orderButton, gpointer data)
{
    GtkTreeIter activeIter;
    GtkTreeModel *tempModel;

    if(!MOTORS_ENABLED)
    {
        // bail out if motor switch is not activated
        show_error_msg("Motors arent activated, start Serial first!");
        cb_reset_toggle_status(orderButton);
        return;
    }

    if(data)
    {
        gboolean available = cb_check_recipe();
        if(!available)
        {
            cb_reset_toggle_status(orderButton);
            show_error_msg("Recipe is not available");
        }
        else
        {
            // TODO: Start order
            g_print("Starting movement...");
        }
    }
    else
    {
        show_error_msg("Error fetching callback data!");
    }
}
