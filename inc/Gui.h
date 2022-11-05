#ifndef __GUI_H_
#define __GUI_H_

/* -- INCLUDES -- */
#include <gtk/gtk.h>
#include <stdbool.h>

/* -- METHODS -- */

// Handles Gtk-Gui Application
int handleGtk();

// Setup of Stack-Page4
void setupPage4(GtkBuilder *builder);

#endif //__GUI_H_