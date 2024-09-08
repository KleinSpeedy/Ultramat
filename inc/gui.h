#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

/**
 * @brief Create all containers holding widgets as well as navigation header
 */
void gui_thread(void);

/**
 * @brief shows a simple modal dialog presenting an error string
 * @param errorStr error string
 */
void gui_show_error_modal(const gchar *errorStr);

#define WIN_WIDTH           1280
#define WIN_HEIGHT          800
#define TRANSITION_SPEED    250 /* ms */

#endif // GUI_H
