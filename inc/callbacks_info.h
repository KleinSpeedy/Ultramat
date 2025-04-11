#ifndef CALLBACKS_INFO_H
#define CALLBACKS_INFO_H

#include <gtk/gtk.h>

struct SemanticVersion
{
    uint8_t major, minor, bugfix;
};

// Set pointer to microcontroller version label
void cb_info_set_mc_version_label(GtkLabel *label);

// Set text of microcontroller version label
void cb_info_set_mc_version(const struct SemanticVersion vers);

// update button for GUI app was clicked
void cb_info_app_update_button_clicked(GtkButton *button);

// check update button for MC firmware was clicked
void cb_info_mc_update_button_clicked(GtkButton *button);

#endif // CALLBACKS_INFO_H
