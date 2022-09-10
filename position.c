#include "position.h"

GtkWidget *pos1Toggle;

void setupPage4(GtkBuilder *builder)
{
    pos1Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos1"));
}

void on_Run_Pos1_toggled(GtkToggleButton *b1)
{
    if (gtk_toggle_button_get_active(b1))
    {
        g_print("Button 1 is ON \n");
    }
    else
    {
        g_print("Button 1 is OFF \n");
    }
}