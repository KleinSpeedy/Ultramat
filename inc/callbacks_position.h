#ifndef CALLBACKS_POSITION_H
#define CALLBACKS_POSITION_H

#include "proto/commands.pb.h"
#include <gtk/gtk.h>
#include <stdint.h>

// vertical position that can be approached on the X axis
typedef enum
{
    PAGES_COMBO_POS_INVALID = -1,
    PAGES_COMBO_POS_ONE, // For pressure pumps, 3 ingredients at pos one
    PAGES_COMBO_POS_TWO,
    PAGES_COMBO_POS_THREE,
    PAGES_COMBO_POS_FOUR,
    PAGES_COMBO_POS_FIVE,
    PAGES_COMBO_POS_SIX,
    PAGES_COMBO_POS_SEVEN,

    PAGES_COMBO_POS_NUM
} ComboPositions_t;

// which pump should be used, 1 - 3
typedef enum
{
    PUMP_POS_INVALID = -1,
    PUMP_POS_ONE, // use this as offset for array indexing, start at 0
    PUMP_POS_TWO,
    PUMP_POS_THREE,

    PUMP_POS_NUM
} ePumpPos_t;

// pos 1 has 3 different drinks as its pumps
#define MAX_INGREDIENT_POSITIONS (PAGES_COMBO_POS_NUM + PUMP_POS_NUM - 1)

/*
 * Positions include 7 approchable positions on the X axis
 * Position one has PUMP_POS_NUM different selectable ingredients
 */
typedef struct ingredient_pos
{
    ComboPositions_t xPos; // x position
    ePumpPos_t pumpPos;    // which pump if pos one is approached
} IngPos_t;

// Get combo position label string for specific position
const char *cb_get_combo_pos_string(const IngPos_t ingPos);

/* ========== CallbackID setter for GTK GUI ========== */

/**
 * @brief Set callback ID of ingredient postion combo widgets
 */
void cb_set_combo_position_callback(GtkComboBox *comboBox,
                                    const IngPos_t ingPos, gulong id);

/**
 * @brief set callback ID and button pointer for manual x position toggle
 * buttons
 */
void cb_set_manual_pos_callback(GtkToggleButton *button, const PositionX pos,
                                const uint64_t id);

/**
 * @brief set callback ID and button pointer for manual y position toggle button
 */
void cb_set_manual_pos_y_callback(GtkToggleButton *button, const gulong id);
/**
 * @brief Set callback ID of recipe order combo widget
 */
void cb_set_combo_order_callback_id(uint64_t id);
// set recipe combo box widget
void cb_set_combo_order_widget(GtkComboBox *widget);

// motor control switch ID setter
void cb_set_motor_switch_callback_id(uint64_t id);
// setter to keep track of motor switch widget
void cb_set_motor_switch_widget(GtkSwitch *widget);

// recipe order button ID setter
void cb_set_button_order_callback_id(uint64_t id);
// setter to keep track of toggle button widget
void cb_set_button_order_widget(GtkToggleButton *widget);
// setter for progress bar widget
void cb_set_progress_bar_widget(GtkProgressBar *widget);
// set the image widget for the ingredient position
void cb_set_combo_position_image(GtkImage *img, const IngPos_t ingPos);

/* ========== GTK Widget event callbacks ========== */

/*
 * @brief Combo box for ingredient position changed
 */
void cb_on_combo_position_changed(GtkComboBox *cBox, void *data);

/*
 * @brief Combo box for ordered recipe changed
 */
void cb_on_combo_order_changed(GtkComboBox *cBox, void *data);

/*
 * @brief motor switch controlling serial connection to microcontroller
 */
int cb_on_motor_switch_toggle(GtkSwitch *motorSwitch, int state, void *data);

/*
 * @brief recipe order toggle button indicating start of drink mixture
 */
void cb_on_recipe_order_toggle(GtkToggleButton *button, void *data);

/*
 * @brief manual x position toggle button was pressed
 */
void cb_on_manual_pos_toggle(GtkToggleButton *button, gpointer data);

/*
 * @brief manual y position toggle button was pressed
 */
void cb_on_manual_pos_y_toggle(GtkToggleButton *button, gpointer data);

/*
 * @brief Reset the position ingredient to no ingredient selected
 */
void cb_on_reset_btn_clicked(GtkButton *button, gpointer data);

/* ========== Callbacks for command done handlers ========== */

// Setup command finished successfully, activate recipe ordering
// NOTE: uses gtk widgets!
gboolean cb_cmd_hello_there_done(gpointer data);

// Recipe step move finished
// NOTE: uses gtk widgets!
gboolean cb_cmd_step_done(gpointer data);

// error during serial communication, stop serial thread from GUI thread
// NOTE: uses gtk widgets!
// TODO: theoretically stops itself
void cb_error_serial_communication(const char *str);

// manual position move on X axis has finished
// NOTE: uses gtk widgets!
gboolean cb_cmd_move_x_done(gpointer data);

// manual position move on Y axis has finished
// NOTE: uses gtk widgets!
gboolean cb_cmd_move_y_done(gpointer data);

/* Util functions */

bool cb_get_position_by_id(const uint8_t id, IngPos_t *ingPos);

#endif // CALLBACKS_POSITION_H
