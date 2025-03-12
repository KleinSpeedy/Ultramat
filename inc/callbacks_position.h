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

/**
 * @brief Set callback ID of ingredient postion combo widgets
 */
void cb_set_combo_position_callback_id(ComboPositions_t pos, uint64_t id);

/*
 * @brief Combo box for ingredient position changed
 */
void cb_on_combo_position_changed(GtkComboBox *cBox, void *data);

/**
 * @brief Set callback ID of recipe order combo widget
 */
void cb_set_combo_order_callback_id(uint64_t id);

/*
 * @brief Combo box for ordered recipe changed
 */
void cb_on_combo_order_changed(GtkComboBox *cBox, void *data);

/* BUTTON - SWITCH CALLBACKS */

void cb_set_motor_switch_callback_id(uint64_t id);

int cb_on_motor_switch_toggle(GtkSwitch *motorSwitch, int state, void *data);

void cb_set_button_order_callback_id(uint64_t id);

void cb_on_recipe_order_toggle(GtkToggleButton *button, void *data);

ComboPositions_t cb_get_position_by_id(uint16_t id);

#endif // CALLBACKS_POSITION_H
