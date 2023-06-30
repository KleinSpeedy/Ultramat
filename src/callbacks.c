/*
 * Implements GTK Widget callbacks and necessary "helper" functions.
 */

/* Project includes */
#include "callbacks.h"
#include "drinklists.h"
#include "serialcom.h"
#include "pages.h"

/* System includes */
#include <glib.h>
#include <string.h>

// Replace current text with default text
// TODO: Use this in pages.c too!
#define RESET_TEXT "Zutat auswählen!"

// get access to main Window
extern GtkWindow *g_mainWindow;

// HACK?: make Handler IDs global
// 6 IDs for 6 callbacks
gulong g_handlerIds[PAGES_COMBO_POS_NUM];

// handler id for orderStart toggle button
gulong g_handlerIdOrderStart;

// handler id for motor switch
gulong g_handlerIdMotorSwitch;

// handler id for recipe order combo box
gulong g_handlerIdComboOrder;

// keep track whether motors were enabled or not
static gboolean MOTORS_ENABLED = FALSE;

/* ========== STATIC FUNCTION DECLARATION ========== */

static void show_error_msg(gchar *errorStr);
static void cb_reset_toggle_status(GtkToggleButton *toggleButton, gulong handlerId);
static void cb_reset_combo_box(GtkComboBox *comboBox, gulong handlerId, GtkTreeIter *toSetIter);
static void cb_set_switch_state(GtkSwitch *sw, gboolean newState);

/* ========== STATIC FUNCTION IMPLEMENTATION ========== */

/**
 * @brief shows a simple modal dialog presenting an error string
 * @todo implement this as helper function in gui.c -> no need for g_mainWindow
 * @param errorStr error string
 */
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

/**
 * @brief Reset the combo box to a former ingredient or to nothing
 * without emitting the changed signal
 * @param comboBox
 * @param handlerId combo handler ID
 * @param toSetIter row in ListStore or NULL if no ingredient on combo position
 */
static void
cb_reset_combo_box(GtkComboBox *comboBox, gulong handlerId, GtkTreeIter *toSetIter)
{
    // block signal emit because we are setting active entry
    g_signal_handler_block(comboBox, handlerId);
    // Reset completely or change back to former selected ingredient
    if(!toSetIter)
    {
        gtk_combo_box_set_active_iter(comboBox, NULL);
        gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboBox))),
                           RESET_TEXT);
    }
    else
    {
        gtk_combo_box_set_active_iter(comboBox, toSetIter);
    }
    // don't forget to unblock signal again
    g_signal_handler_unblock(comboBox, handlerId);
}

/* TODO: Implement this when properly implementing the position property
 * see git commit log for implementation
 *
static gint on_already_selected(const gchar *name, guint position);
*/

// Change order toggle button status back to inactive
// without emitting signal
static void
cb_reset_toggle_status(GtkToggleButton *toggleButton, gulong handlerId)
{
    g_signal_handler_block(toggleButton, handlerId);
    gtk_toggle_button_set_active(toggleButton, FALSE);
    g_signal_handler_unblock(toggleButton, handlerId);
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

/* ========== CALLBACK IMPLEMENTATIONS ========== */

/**
 * @brief Callback for selecting a new Ingredient on a ComboBox
 * This callback needs to handle a possible reselection too
 *
 * @param comboBox callback ComboBox
 * @param data position of combobox as pointer data
 */
void
on_combo_pos_changed(GtkComboBox *comboBox, gpointer data)
{
    GtkTreeModel *comboModel;
    GtkTreeIter  activeIter, *formerIter;
    UIngredient *activeIng, *formerIng;

    // Get combo position num from callback data
    gint8 comboPosition = GPOINTER_TO_INT(data);
    // Get list store data
    comboModel = gtk_combo_box_get_model(comboBox);
    gtk_combo_box_get_active_iter(comboBox, &activeIter);
    gtk_tree_model_get(comboModel, &activeIter,
                       ING_COLUMN_OBJECT, &activeIng,
                       -1); // terminate

    // Save former Ingredient of position if there is one
    formerIng = lists_ingredient_get_by_position(comboPosition);

    // Check if active ingredient already holds a position
    if(u_ingredient_get_selected(activeIng))
    {
        show_error_msg("Ingredient is already selected elsewhere!");
        if(formerIng == NULL)
        {
            // position held no ingredient before
            cb_reset_combo_box(comboBox, g_handlerIds[comboPosition], NULL);
        }
        else
        {
            // Reset position to old ingredient
            formerIter = lists_iter_get_by_position(u_ingredient_get_position(formerIng));
            cb_reset_combo_box(comboBox, g_handlerIds[comboPosition], formerIter);
            gtk_tree_iter_free(formerIter);
        }

        g_object_unref(formerIng);
        g_object_unref(activeIng);
        return;
    }
    // No Ingredient holds current position
    if(formerIng == NULL)
    {
        u_ingredient_is_selected(activeIng, comboPosition);
        g_object_unref(activeIng);
        return;
    }
    else
    {
        // If it's the same ingredient do nothing, as it should not be possible
        if(u_ingredient_get_id(activeIng) == u_ingredient_get_id(formerIng))
        {
            g_object_unref(formerIng);
            g_object_unref(activeIng);
            return;
        }
        // unset former Ingredient holding position
        u_ingredient_set_unselected(formerIng);
        // set new ingredient as selected
        u_ingredient_is_selected(activeIng, comboPosition);

        g_object_unref(formerIng);
        g_object_unref(activeIng);
        return;
    }
}

gboolean
on_motor_switch_toggle(GtkSwitch *motorSwitch, gboolean state, gpointer data)
{
    (void) data;
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

    URecipe *activeRecipe = NULL, *formerRecipe = NULL;
    GtkTreeIter activeIter;
    GtkTreeModel *comboModel = gtk_combo_box_get_model(comboBox);
    gtk_combo_box_get_active_iter(comboBox, &activeIter);

    if((formerRecipe = lists_recipe_get_active_recipe()) != NULL)
    {
        u_recipe_set_selected(formerRecipe, FALSE);
        g_object_unref(formerRecipe);
    }
    else
    {
        g_print("Callback: No formerly active recipe!\n");
    }

    gtk_tree_model_get(comboModel, &activeIter,
                       REC_COLUMN_OBJECT, &activeRecipe,
                       -1); // terminate
    // TODO: This does not print the full name, but it gets displayed right in the combo box -> WHY?!
    g_print("Selected Recipe: %s %d\n",
            u_recipe_get_name(activeRecipe), u_recipe_get_id(activeRecipe));
    u_recipe_set_selected(activeRecipe, TRUE);

    if(activeRecipe)
        g_object_unref(activeRecipe);
}

// gets called when starting a new mix and checks if all ingredients 
// for the recipe are available, if so it needs to send the serial commands
void
on_recipe_order_toggle(GtkToggleButton *orderButton, gpointer data)
{
    (void) data;
    /*
    if(!MOTORS_ENABLED)
    {
        // bail out if motor switch is not activated
        show_error_msg("Motors are not activated, start Serial first!");
        cb_reset_toggle_status(orderButton, g_handlerIdComboOrder);
        return;
    }
    */
    URecipe *activeRecipe = lists_recipe_get_active_recipe();

    if(activeRecipe == NULL)
    {
        cb_reset_toggle_status(orderButton, g_handlerIdComboOrder);
        show_error_msg("Error fetching recipe information!");
        g_object_unref(activeRecipe);
        return;
    }

    gboolean available = u_recipe_is_available(activeRecipe);
    if(!available)
    {
        cb_reset_toggle_status(orderButton, g_handlerIdComboOrder);
        show_error_msg("Recipe is not available!");
    }
    else
    {
        g_print("Starting movement...");
    }

    g_object_unref(activeRecipe);
}
