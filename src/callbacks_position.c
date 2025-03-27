#include "callbacks_position.h"
#include "drinks.h"
#include "drinklists.h"
#include "gui.h"
#include "serialcomms.h"

#include <glibconfig.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

// This array holds as many ingredients as there are approachable positions
static const Ingredient *activeIngredients_[PAGES_COMBO_POS_NUM];

// The recipe selected by the order recipe combo box
static Recipe *activeRecipe_ = NULL;
static uint16_t recipeStepDoneCounter_ = 0;

// Callback IDs for ingredient combo boxes and recipe combo box
static gulong comboCbIds_[PAGES_COMBO_POS_NUM];
static gulong recipeComboId_;

// callback IDs for recipe order toggle button and motor switch
static gulong buttonOrderCbId_;
static gulong switchMotorCbId_;

static GtkComboBox *recipeComboBox_ = NULL;
static GtkToggleButton *recipeOrderButton_ = NULL;
static GtkSwitch *motorSwitch_ = NULL;
static GtkProgressBar *progressBar_ = NULL;

#define ALREADY_SELECTED_ERROR "Zutat bereits ausgewählt!"

static void cb_reset_combo_box(GtkComboBox *comboBox, gulong handlerId)
{
    // block signal emit because we are setting active entry
    g_signal_handler_block(comboBox, handlerId);
    // Reset completely or change back to former selected ingredient
    gtk_combo_box_set_active_iter(comboBox, NULL);
    gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboBox))),
            RESET_TEXT);
    // don't forget to unblock signal again
    g_signal_handler_unblock(comboBox, handlerId);
}

static void cb_reset_toggle_button(GtkToggleButton *tb, gulong handlerId)
{
    g_signal_handler_block(tb, handlerId);
    gtk_toggle_button_set_active(tb, FALSE);
    g_signal_handler_unblock(tb, handlerId);
}

static void cb_set_switch_state(GtkSwitch *motorSwitch, gboolean state)
{
    g_signal_handler_block(motorSwitch, switchMotorCbId_);
    gtk_switch_set_state(motorSwitch, state);
    gtk_switch_set_active(motorSwitch, state);
    g_signal_handler_unblock(motorSwitch, switchMotorCbId_);
}

static void cb_set_ing_pos(const Ingredient *ing, const int pos)
{
    assert(pos >= 0 && pos < PAGES_COMBO_POS_NUM && ing != NULL);
    activeIngredients_[pos] = ing;
}

static const Ingredient *cb_get_ing_pos(int pos)
{
    assert(pos >= 0 && pos < PAGES_COMBO_POS_NUM);
    return activeIngredients_[pos];
}

// TODO: Keep return values consistent across project
static int cb_ingredient_already_exists(const Ingredient *ing)
{
    if(ing == NULL)
        return 0;

    for(int i = 0; i < PAGES_COMBO_POS_NUM; i++)
    {
        const Ingredient *temp = cb_get_ing_pos(i);
        if(temp == NULL)
            continue;
        if(temp->id == ing->id)
            return 1;
    }
    return 0;
}

/* PUBLIC FUNCTIONS */

void cb_set_combo_position_callback_id(ComboPositions_t pos, uint64_t id)
{
    /* ID always greater 0 for successfull connections */
    assert(id > 0);
    assert(pos != PAGES_COMBO_POS_INVALID && pos < PAGES_COMBO_POS_NUM);
    if(pos == PAGES_COMBO_POS_INVALID || pos >= PAGES_COMBO_POS_NUM)
    {
        // TOOD: Proper error log
        fprintf(stderr, "Error: Out of bounds callback id\n");
        return;
    }
    comboCbIds_[pos] = id;
}

void cb_on_combo_position_changed(GtkComboBox *comboBox, gpointer data)
{
    GtkTreeModel *treeModel = gtk_combo_box_get_model(comboBox);
    GtkTreeIter activeIter;
    uint16_t id;
    gtk_combo_box_get_active_iter(comboBox, &activeIter);
    gtk_tree_model_get(treeModel, &activeIter,
                       ING_COLUMN_ID, &id,
                       -1); // terminate
    Ingredient *selectedIng = drinks_io_get_ingredient_by_id(id);

    if(cb_get_ing_pos(GPOINTER_TO_INT(data)) == NULL)
    {
        // set ingredient to position if it doesnt already exist
        if(!cb_ingredient_already_exists(selectedIng))
        {
            cb_set_ing_pos(selectedIng, GPOINTER_TO_INT(data));
        }
        else
        {
            cb_reset_combo_box(comboBox, comboCbIds_[GPOINTER_TO_INT(data)]);
            gui_show_error_modal(ALREADY_SELECTED_ERROR);
        }
        return;
    }

    if(cb_ingredient_already_exists(selectedIng))
    {
        cb_reset_combo_box(comboBox, comboCbIds_[GPOINTER_TO_INT(data)]);
        gui_show_error_modal(ALREADY_SELECTED_ERROR);
        return;
    }

    // reset the ingredient on position
    cb_set_ing_pos(selectedIng, GPOINTER_TO_INT(data));
}

void cb_set_combo_order_callback_id(uint64_t id)
{
    /* ID always greater 0 for successfull connections */
    assert(id > 0);
    recipeComboId_ = id;
}

void cb_set_combo_order_widget(GtkComboBox *widget)
{
    assert(widget != NULL);
    recipeComboBox_ = widget;
    g_object_add_weak_pointer(G_OBJECT(widget), (gpointer *)&recipeComboBox_);
}

void cb_on_combo_order_changed(GtkComboBox *comboBox, void *data)
{
    (void) data;

    GtkTreeModel *treeModel = gtk_combo_box_get_model(comboBox);
    GtkTreeIter activeIter;
    uint16_t id;
    gtk_combo_box_get_active_iter(comboBox, &activeIter);
    gtk_tree_model_get(treeModel, &activeIter,
                       REC_COLUMN_ID, &id,
                       -1); // terminate

    Recipe *rec = drinks_io_get_recipe_by_id(id);
    activeRecipe_ = rec;
}

void cb_set_button_order_callback_id(uint64_t id)
{
    /* ID always greater 0 for successfull connections */
    assert(id > 0);
    buttonOrderCbId_ = id;
}

void cb_set_button_order_widget(GtkToggleButton *widget)
{
    assert(widget != NULL);
    recipeOrderButton_ = widget;
    g_object_add_weak_pointer(G_OBJECT(widget), (gpointer *)&recipeOrderButton_);
}

void cb_on_recipe_order_toggle(GtkToggleButton *button, void *data)
{
    (void) data;

    // dont do anything if toggled back to inactive
    if(!gtk_toggle_button_get_active(button))
    {
        return;
    }
    if(!gtk_widget_is_sensitive(GTK_WIDGET(button)))
    {
        printf("order button not activated\n");
        return;
    }

    if(activeRecipe_ == NULL)
    {
        cb_reset_toggle_button(button, buttonOrderCbId_);
        gui_show_error_modal("Kein Rezept ausgewählt!");
        return;
    }
    // TODO: Check if serial connection active

    // check if all ingredients of recipe are active
    for(int i = 0; i < activeRecipe_->ingCount; i++)
    {
        IdCountPair pair = activeRecipe_->ingPairs[i];
        Ingredient *ing = drinks_io_get_ingredient_by_id(pair.id);
        if(!cb_ingredient_already_exists(ing))
        {
            cb_reset_toggle_button(button, buttonOrderCbId_);
            gui_show_error_modal("Es fehlt eine Zutat!");
            return;
        }
    }

    // Disable combo box and toggle button:
    gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(recipeComboBox_), FALSE);
    if(comms_start_new_mixing(activeRecipe_) < 0)
    {
        printf("start mixing failed\n");
        // TODO: Reset everything here
    }
}

void cb_set_motor_switch_callback_id(uint64_t id)
{
    /* ID always greater 0 for successfull connections */
    assert(id > 0);
    switchMotorCbId_ = id;
}

void cb_set_motor_switch_widget(GtkSwitch *widget)
{
    assert(widget != NULL);
    motorSwitch_ = widget;
    g_object_add_weak_pointer(G_OBJECT(widget), (gpointer *)&motorSwitch_);
}

int cb_on_motor_switch_toggle(GtkSwitch *motorSwitch, int state, void *data)
{
    (void) data;

    if(state)
    {
        if(comms_start_serial_connection() < 0)
        {
            gui_show_error_modal("Fehler bei Kommunikationsaufbau");
            cb_set_switch_state(motorSwitch, FALSE);
            return TRUE;
        }
        cb_set_switch_state(motorSwitch, TRUE);
    }
    else
    {
        printf("False entered\n");
        comms_stop_serial_connection();
        gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), FALSE);
        cb_set_switch_state(motorSwitch, FALSE);
    }

    return TRUE;
}

void cb_set_progress_bar_widget(GtkProgressBar *widget)
{
    assert(widget != NULL);
    progressBar_ = widget;
    g_object_add_weak_pointer(G_OBJECT(widget), (gpointer *)&progressBar_);
}

ComboPositions_t cb_get_position_by_id(const uint8_t id)
{
    for(ComboPositions_t pos = 0;
        pos < PAGES_COMBO_POS_NUM;
        ++pos)
    {
        if(!activeIngredients_[pos])
            continue;
        if(activeIngredients_[pos]->id == id)
            return pos;
    }

    return PAGES_COMBO_POS_INVALID;
}

/* Callbacks for command done handlers */

void cb_cmd_hello_there_done(void)
{
    gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), TRUE);
}

void cb_cmd_move_done(void)
{
    recipeStepDoneCounter_++;
    const gdouble progressWidth = (1.0f / activeRecipe_->ingCount) * recipeStepDoneCounter_;
    // TODO: Floating point comparison
    // make sure we do not pass something above 1.0
    if(progressWidth >= 1.0f)
        gtk_progress_bar_set_fraction(progressBar_, .999f);
    else
        gtk_progress_bar_set_fraction(progressBar_, progressWidth);

    if(recipeStepDoneCounter_ == activeRecipe_->ingCount)
    {
        recipeStepDoneCounter_ = 0;
        gtk_widget_set_sensitive(GTK_WIDGET(recipeComboBox_), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), TRUE);
        cb_reset_toggle_button(recipeOrderButton_, buttonOrderCbId_);

        gui_show_info_modal("Drink ist fertig!");

        gtk_progress_bar_set_fraction(progressBar_, 0.0f);
    }
}

void cb_error_serial_communication(const char *str)
{
    // stop serial connection, disable order button and reset switch
    comms_stop_serial_connection();
    gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(recipeComboBox_), TRUE);
    cb_set_switch_state(motorSwitch_, FALSE);

    char errStr[128] = {0};
    snprintf(errStr, sizeof(errStr), "Fehler bei Kommunikation: %s", str);
    gui_show_error_modal(errStr);
}
