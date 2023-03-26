#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <gtk/gtk.h>

gboolean on_motor_switch_toggle(GtkSwitch *motorSwitch, gboolean state, gpointer data);

void on_combo_pos_changed(GtkComboBox *comboBox, gpointer data);

void on_drink_order_toggle(GtkToggleButton *orderButton, gpointer data);

#endif //__CALLBACKS_H__

