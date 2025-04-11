#ifndef CALLBACKS_POSITION_H
#define CALLBACKS_POSITION_H

#include <stdint.h>
#include <gtk/gtk.h>

/*
 * ComboBox positions on page one, used for callbacks and keeping track of
 * ingredient positions
 */
typedef enum
{
    PAGES_COMBO_POS_INVALID = -1,
    PAGES_COMBO_POS_ONE,
    PAGES_COMBO_POS_TWO,
    PAGES_COMBO_POS_THREE,
    PAGES_COMBO_POS_FOUR,
    PAGES_COMBO_POS_FIVE,
    PAGES_COMBO_POS_SIX,
    //PAGES_COMBO_POS_SEVEN, //TODO: Add this when adding pressure pumps

    PAGES_COMBO_POS_NUM
} ComboPositions_t;

/* ========== CallbackID setter for GTK GUI ========== */

/**
 * @brief Set callback ID of ingredient postion combo widgets
 */
void cb_set_combo_position_callback_id(ComboPositions_t pos, uint64_t id);

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

/* ========== Callbacks for command done handlers ========== */

// Setup command finished successfully, activate recipe ordering
void cb_cmd_hello_there_done(const uint8_t *payload, const uint8_t size);

// Recipe step move finished
void cb_cmd_move_done(void);

// error during serial communication, stop serial thread from GUI thread
void cb_error_serial_communication(const char *str);

/* Util functions */

ComboPositions_t cb_get_position_by_id(const uint8_t id);

#endif // CALLBACKS_POSITION_H
