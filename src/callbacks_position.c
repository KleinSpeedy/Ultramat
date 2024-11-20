#include "callbacks_position.h"
#include "drinks.h"
#include "drinklists.h"
#include "gui.h"

#include <glibconfig.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

// This array holds as many ingredients as there are approachable positions
static const Ingredient *activeIngredients_[PAGES_COMBO_POS_NUM];

// The recipe selected by the order recipe combo box
static Recipe *activeRecipe_;

static gulong comboCbIds_[PAGES_COMBO_POS_NUM];
static gulong comboOrderCbId_;

static gulong buttonOrderCbId_;
static gulong switchMotorCbId_;

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

static inline void cb_set_ing_pos(const Ingredient *ing, int pos)
{
    assert(pos >= 0 && pos < PAGES_COMBO_POS_NUM);
    activeIngredients_[pos] = ing;
}

static inline const Ingredient *cb_get_ing_pos(int pos)
{
    assert(pos >= 0 && pos < PAGES_COMBO_POS_NUM);
    return activeIngredients_[pos];
}

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

// TODO: Somehow data always is 0 if used in a separate variable with macro
// using it directly with macro solves this problem
// TODO-2: Something is seriously wrong with glong and gulong or long to int
// conversion from pointer in general
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
    comboOrderCbId_ = id;
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

void cb_on_recipe_order_toggle(GtkToggleButton *button, void *data)
{
    // user data not used
    (void) data;

    if(!gtk_toggle_button_get_active(button))
    {
        return;
    }

    if(activeRecipe_ == NULL)
    {
        cb_reset_toggle_button(button, buttonOrderCbId_);
        gui_show_error_modal("Kein Rezept ausgewählt!");
        return;
    }

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

    printf("Start mixing\n");
}

void cb_set_motor_switch_callback_id(uint64_t id)
{
    /* ID always greater 0 for successfull connections */
    assert(id > 0);
    switchMotorCbId_ = id;
}

int cb_on_motor_switch_toggle(GtkSwitch *motorSwitch, int state, void *data)
{
    (void) motorSwitch;
    (void) state;
    (void) data;

    return TRUE;
}
