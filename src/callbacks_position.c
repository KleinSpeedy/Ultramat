#include "callbacks_position.h"
#include "callbacks_info.h"
#include "drinklists.h"
#include "drinks.h"
#include "glib.h"
#include "gtk/gtk.h"
#include "gui.h"
#include "message_handler.h"
#include "proto/commands.pb.h"
#include "proto_serial.h"

#include <assert.h>
#include <stdio.h>

#define IMG_PIXEL_SIZE 128
#define ALREADY_SELECTED_ERROR "Zutat bereits ausgewählt!"

#define TO_ING_POS(x) *(IngPos_t *)(x)

// This array holds as many ingredients as there are approachable positions
// with the exception of the final position
static const Ingredient *activeIngredients_[MAX_INGREDIENT_POSITIONS];

// The recipe selected by the order recipe combo box
static Recipe *activeRecipe_ = NULL;
static uint16_t recipeStepDoneCounter_ = 0;

// Callback IDs for ingredient combo boxes and recipe combo box
static gulong comboCbIds_[MAX_INGREDIENT_POSITIONS];
static gulong recipeComboId_;
// pointer to combo boxes for positions
static GtkComboBox *comboBoxs_[MAX_INGREDIENT_POSITIONS];
// pointer to icons for positions
static GtkImage *images_[MAX_INGREDIENT_POSITIONS];

// Callback ids for manual position toggle buttons, we have 8 positions
static gulong manualPosButtonIds_[PositionX_FINAL];
static GtkToggleButton *manualPosButtons_[PositionX_FINAL];

// id and widget pointer for manual y button
static gulong manualPosYButtonId_;
static GtkToggleButton *manualPosYButton_;

// callback IDs for recipe order toggle button and motor switch
static gulong buttonOrderCbId_;
static gulong switchMotorCbId_;

static GtkComboBox *recipeComboBox_ = NULL;
static GtkToggleButton *recipeOrderButton_ = NULL;
static GtkSwitch *motorSwitch_ = NULL;
static GtkProgressBar *progressBar_ = NULL;

static int cb_get_array_index(const IngPos_t ingPos)
{
    assert(ingPos.xPos != PAGES_COMBO_POS_INVALID ||
           ingPos.pumpPos != PUMP_POS_INVALID);

    if(ingPos.xPos == PAGES_COMBO_POS_ONE)
    {
        return ingPos.xPos + ingPos.pumpPos;
    }
    // first barbutler position is array position 3
    return 2 + ingPos.xPos;
}

static void cb_reset_ingredient(const int pos)
{
    assert(pos > PAGES_COMBO_POS_INVALID && pos < PAGES_COMBO_POS_NUM);
    activeIngredients_[pos] = NULL;
}

static void cb_set_ing_pos(const Ingredient *ing, const int pos)
{
    assert(pos >= 0 && pos < MAX_INGREDIENT_POSITIONS && ing != NULL);
    activeIngredients_[pos] = ing;
}

static const Ingredient *cb_get_ing_pos(const int pos)
{
    assert(pos >= 0 && pos < MAX_INGREDIENT_POSITIONS);
    return activeIngredients_[pos];
}

static void cb_clear_pos_img(const int pos)
{
    gtk_image_clear(images_[pos]);
    // set default icon
    gtk_image_set_from_pixbuf(
        images_[pos],
        gtk_icon_theme_load_icon(gtk_icon_theme_get_default(), "image-loading",
                                 IMG_PIXEL_SIZE, 0, NULL));
}

static void cb_reset_combo_box(GtkComboBox *comboBox, gulong handlerId,
                               const int pos)
{
    // block signal emit because we are setting active entry
    g_signal_handler_block(comboBox, handlerId);
    // Reset completely
    gtk_combo_box_set_active_iter(comboBox, NULL);
    gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboBox))),
                       RESET_TEXT);
    cb_clear_pos_img(pos);
    cb_reset_ingredient(pos);
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

// TODO: Keep return values consistent across project
static int cb_ingredient_already_exists(const Ingredient *ing)
{
    if(ing == NULL)
        return 0;

    for(int i = 0; i < MAX_INGREDIENT_POSITIONS; i++)
    {
        const Ingredient *temp = cb_get_ing_pos(i);
        if(temp == NULL)
            continue;
        if(temp->id == ing->id)
            return 1;
    }
    return 0;
}

static void cb_set_manual_pos_sensitive_state(const gboolean state)
{
    // off by one is ok, as final is also num positions
    for(int i = 0; i < PositionX_FINAL; i++)
    {
        // if we enable them, reset them too
        if(state == TRUE)
        {
            cb_reset_toggle_button(manualPosButtons_[i],
                                   manualPosButtonIds_[i]);
        }
        gtk_widget_set_sensitive(GTK_WIDGET(manualPosButtons_[i]), state);
    }
}

static void cb_set_pos_img(GdkPixbuf *pixbuf, const int pos)
{
    if(!pixbuf)
    {
        g_warning("Could not load pixbuf");
        return;
    }

    gtk_image_clear(images_[pos]);
    gtk_image_set_from_pixbuf(images_[pos], pixbuf);
}

/* PUBLIC FUNCTIONS */

const char *cb_get_combo_pos_string(const IngPos_t ingPos)
{
    assert(ingPos.xPos > PAGES_COMBO_POS_INVALID &&
           ingPos.xPos < PAGES_COMBO_POS_NUM);

    const int pos = cb_get_array_index(ingPos);
    assert(pos > PAGES_COMBO_POS_INVALID && pos < MAX_INGREDIENT_POSITIONS);

    static const char *comboPosStrMap[MAX_INGREDIENT_POSITIONS] = {
        "Pumpe 1",     "Pumpe 2",     "Pumpe 3",
        "Barbutler 1", "Barbutler 2", "Barbutler 3",
        "Barbutler 4", "Barbutler 5", "Barbutler 6"};
    return comboPosStrMap[pos];
}

void cb_set_combo_position_callback(GtkComboBox *comboBox,
                                    const IngPos_t ingPos, gulong id)
{
    assert(comboBox != NULL);
    /* ID always greater 0 for successfull connections */
    assert(id > 0);
    assert(ingPos.xPos > PAGES_COMBO_POS_INVALID &&
           ingPos.xPos < PAGES_COMBO_POS_NUM);

    const int pos = cb_get_array_index(ingPos);

    comboBoxs_[pos] = comboBox;
    comboCbIds_[pos] = id;
    g_object_add_weak_pointer(G_OBJECT(comboBox), (gpointer *)&comboBoxs_[pos]);
}

void cb_set_manual_pos_callback(GtkToggleButton *button, const PositionX pos,
                                const uint64_t id)
{
    /* ID always greater 0 for successfull connections */
    assert(id > 0);
    assert(button != NULL);
    assert(pos != PositionX_INVALID && pos <= PositionX_FINAL);
    const uint8_t i = (uint8_t)(pos - 1);
    manualPosButtonIds_[i] = id;

    // add reference so it doesnt get cleared by glib
    manualPosButtons_[i] = button;
    g_object_add_weak_pointer(G_OBJECT(button),
                              (gpointer *)&manualPosButtons_[i]);
}

void cb_set_manual_pos_y_callback(GtkToggleButton *button, const gulong id)
{
    assert(id > 0);
    assert(button != NULL);

    manualPosYButtonId_ = id;
    manualPosYButton_ = button;
    g_object_add_weak_pointer(G_OBJECT(button), (gpointer *)&manualPosYButton_);
}

void cb_on_combo_position_changed(GtkComboBox *comboBox, gpointer data)
{
    assert(data != NULL);

    GtkTreeModel *treeModel = gtk_combo_box_get_model(comboBox);
    GtkTreeIter activeIter;
    uint16_t id;
    GdkPixbuf *pixbuf;
    gtk_combo_box_get_active_iter(comboBox, &activeIter);
    gtk_tree_model_get(treeModel, &activeIter, ING_COLUMN_ID, &id,
                       ING_COLUMN_ICON, &pixbuf,
                       -1); // terminate
    Ingredient *selectedIng = drinks_io_get_ingredient_by_id(id);

    const IngPos_t ingPos = TO_ING_POS(data);
    const int pos = cb_get_array_index(ingPos);

    if(cb_get_ing_pos(pos) == NULL)
    {
        // set ingredient to position if it doesnt already exist
        if(!cb_ingredient_already_exists(selectedIng))
        {
            cb_set_ing_pos(selectedIng, pos);
            cb_set_pos_img(pixbuf, pos);
        }
        else
        {
            cb_reset_combo_box(comboBox, comboCbIds_[pos], pos);
            gui_show_error_modal(ALREADY_SELECTED_ERROR);
        }
        return;
    }

    if(cb_ingredient_already_exists(selectedIng))
    {
        cb_reset_combo_box(comboBox, comboCbIds_[pos], pos);
        gui_show_error_modal(ALREADY_SELECTED_ERROR);
        return;
    }

    // ingredient changed
    cb_set_ing_pos(selectedIng, pos);
    cb_set_pos_img(pixbuf, pos);
}

void cb_on_reset_btn_clicked(GtkButton *button, gpointer data)
{
    (void)button;

    const IngPos_t ingPos = TO_ING_POS(data);
    const int pos = cb_get_array_index(ingPos);

    cb_reset_combo_box(comboBoxs_[pos], comboCbIds_[pos], pos);
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
    (void)data;

    GtkTreeModel *treeModel = gtk_combo_box_get_model(comboBox);
    GtkTreeIter activeIter;
    uint16_t id;
    gtk_combo_box_get_active_iter(comboBox, &activeIter);
    gtk_tree_model_get(treeModel, &activeIter, REC_COLUMN_ID, &id,
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
    g_object_add_weak_pointer(G_OBJECT(widget),
                              (gpointer *)&recipeOrderButton_);
}

void cb_on_recipe_order_toggle(GtkToggleButton *button, void *data)
{
    (void)data;

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
    if(!proto_comms_thread_running())
    {
        cb_reset_toggle_button(button, buttonOrderCbId_);
        gui_show_error_modal("Serial thread nicht aktiv!");
        return;
    }

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
    if(proto_comms_start_new_mixing(activeRecipe_) < 0)
    {
        printf("start mixing failed\n");
        // TODO: Reset everything here
    }
}

void cb_on_manual_pos_toggle(GtkToggleButton *button, gpointer data)
{
    if(!proto_comms_thread_running())
    {
        gui_show_error_modal("Serial thread nicht aktiv!");
        // index is always of by 1 for manual pos buttons
        const uint8_t i = GPOINTER_TO_INT(data) - 1;
        cb_reset_toggle_button(button, manualPosButtonIds_[i]);
        return;
    }

    cb_set_manual_pos_sensitive_state(FALSE);
    message_handler_move_x(GPOINTER_TO_INT(data));
}

void cb_on_manual_pos_y_toggle(GtkToggleButton *button, gpointer data)
{
    (void)data;
    if(!proto_comms_thread_running())
    {
        gui_show_error_modal("Serial thread nicht aktiv!");
        cb_reset_toggle_button(button, manualPosYButtonId_);
        return;
    }

    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
    // just move once
    message_handler_move_y(1);
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
    (void)data;

    if(state)
    {
        if(proto_comms_start_serial_connection() < 0)
        {
            gui_show_error_modal("Fehler bei Kommunikationsaufbau");
            cb_set_switch_state(motorSwitch, FALSE);
            return TRUE;
        }
        cb_set_switch_state(motorSwitch, TRUE);
    }
    else
    {
        proto_comms_stop_serial_connection();
        gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), FALSE);
        cb_set_switch_state(motorSwitch, FALSE);
    }

    return TRUE;
}

void cb_deactivate_motor_switch(void)
{
    // TODO: reset here
}

void cb_set_progress_bar_widget(GtkProgressBar *widget)
{
    assert(widget != NULL);
    progressBar_ = widget;
    g_object_add_weak_pointer(G_OBJECT(widget), (gpointer *)&progressBar_);
}

void cb_set_combo_position_image(GtkImage *img, const IngPos_t ingPos)
{
    assert(img != NULL);
    assert(ingPos.xPos > PAGES_COMBO_POS_INVALID &&
           ingPos.xPos < PAGES_COMBO_POS_NUM);

    const int pos = cb_get_array_index(ingPos);

    // set default size
    gtk_image_set_pixel_size(img, IMG_PIXEL_SIZE);
    images_[pos] = img;
    // clear and set default icon
    cb_clear_pos_img(pos);
    g_object_add_weak_pointer(G_OBJECT(img), (gpointer)&images_[pos]);
}

bool cb_get_position_by_id(const uint8_t id, IngPos_t *ingPos)
{
    if(!ingPos)
        return FALSE;

    int pos = -1;
    for(int i = 0; i < MAX_INGREDIENT_POSITIONS; i++)
    {
        if(!activeIngredients_[i])
            continue;
        if(activeIngredients_[i]->id == id)
        {
            pos = i;
            break;
        }
    }
    if(pos < 0)
    {
        return FALSE;
    }
    // check if pump position
    if(pos >= 0 && pos < PUMP_POS_NUM)
    {
        ingPos->xPos = PAGES_COMBO_POS_ONE;
        ingPos->pumpPos = (ePumpPos_t)pos;
    }
    else
    {
        ingPos->pumpPos = PUMP_POS_INVALID;
        ingPos->xPos = (ComboPositions_t)pos;
    }

    return TRUE;
}

/* Callbacks for command done handlers */

gboolean cb_cmd_hello_there_done(gpointer data)
{
    const Response *resp = data;
    // activate recipe order
    gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), TRUE);

    // setting default version v0.0.1 in case payload is missing
    struct SemanticVersion vers = {0, 0, 1};
    if(!resp || !resp->has_version)
    {
        gui_show_error_modal("Could not read real microcontroller version!");
    }
    else
    {
        vers.major = resp->version.major;
        vers.minor = resp->version.minor;
        vers.bugfix = resp->version.bugfix;
    }

    cb_info_set_mc_version(vers);
    g_free(data);
    return G_SOURCE_REMOVE;
}

gboolean cb_cmd_step_done(gpointer data)
{
    (void)data;
    // dont forget move to final position
    const uint16_t maxMoves = activeRecipe_->ingCount + 1;

    recipeStepDoneCounter_++;
    const gdouble progressWidth = (1.0f / maxMoves) * recipeStepDoneCounter_;

    // TODO: Floating point comparison
    // make sure we do not pass something above 1.0
    if(progressWidth >= 1.0f)
        gtk_progress_bar_set_fraction(progressBar_, .999f);
    else
        gtk_progress_bar_set_fraction(progressBar_, progressWidth);

    if(recipeStepDoneCounter_ == maxMoves)
    {
        recipeStepDoneCounter_ = 0;
        gtk_widget_set_sensitive(GTK_WIDGET(recipeComboBox_), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), TRUE);
        cb_reset_toggle_button(recipeOrderButton_, buttonOrderCbId_);

        gtk_progress_bar_set_fraction(progressBar_, 0.0f);

        gui_show_info_modal("Drink ist fertig!");
    }

    return G_SOURCE_REMOVE;
}

gboolean cb_cmd_move_x_done(gpointer data)
{
    (void)data;
    // enable all manual pos buttons after move is done
    cb_set_manual_pos_sensitive_state(TRUE);

    return G_SOURCE_REMOVE;
}

gboolean cb_cmd_move_y_done(gpointer data)
{
    (void)data;
    cb_reset_toggle_button(manualPosYButton_, manualPosYButtonId_);
    gtk_widget_set_sensitive(GTK_WIDGET(manualPosYButton_), TRUE);

    return G_SOURCE_REMOVE;
}

void cb_error_serial_communication(const char *str)
{
    // stop serial connection, disable order button and reset switch
    // WARNING: serial thread cancels itself here!
    // proto_comms_stop_serial_connection();
    gtk_widget_set_sensitive(GTK_WIDGET(recipeOrderButton_), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(recipeComboBox_), TRUE);
    cb_set_switch_state(motorSwitch_, FALSE);

    if(!str)
    {
        gui_show_error_modal("Fehler bei Kommunikation");
    }
    else
    {
        char errStr[128] = {0};
        snprintf(errStr, sizeof(errStr), "Fehler bei Kommunikation: %s", str);
        gui_show_error_modal(errStr);
    }
}
