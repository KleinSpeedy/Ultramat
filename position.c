#include "position.h"

// Global Toggle Buttons

GtkWidget *pos1Toggle;
GtkWidget *pos2Toggle;
GtkWidget *pos3Toggle;
GtkWidget *pos4Toggle;
GtkWidget *pos5Toggle;
GtkWidget *pos6Toggle;
GtkWidget *pos7Toggle;

/* setup Stack Page 4, initialize all toggle Buttons */
void setupPage4(GtkBuilder *builder)
{
    pos1Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos1"));
    pos2Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos2"));
    pos3Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos3"));
    pos4Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos4"));
    pos5Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos5"));
    pos6Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos6"));
    pos7Toggle = GTK_WIDGET(gtk_builder_get_object(builder,"Run_Pos7"));
}

void on_Run_Pos1_toggled(GtkToggleButton *toggle_button)
{

    if (gtk_toggle_button_get_active(toggle_button))
    {
        g_print("Button 1 is ON \n");
    }
    else
    {
        g_print("Button 1 is OFF \n");
    }
}

void on_Run_Pos2_toggled(GtkToggleButton *toggle_button)
{
    if (gtk_toggle_button_get_active(toggle_button))
    {
        g_print("Button 2 is ON \n");
    }
    else
    {
        g_print("Button 2 is OFF \n");
    }
}

void on_Run_Pos3_toggled(GtkToggleButton *toggle_button)
{
    if (gtk_toggle_button_get_active(toggle_button))
    {
        g_print("Button 3 is ON \n");
    }
    else
    {
        g_print("Button 3 is OFF \n");
    }
}

void on_Run_Pos4_toggled(GtkToggleButton *toggle_button)
{
    if (gtk_toggle_button_get_active(toggle_button))
    {
        g_print("Button 4 is ON \n");
    }
    else
    {
        g_print("Button 4 is OFF \n");
    }
}

void on_Run_Pos5_toggled(GtkToggleButton *toggle_button)
{
    if (gtk_toggle_button_get_active(toggle_button))
    {
        g_print("Button 5 is ON \n");
    }
    else
    {
        g_print("Button 5 is OFF \n");
    }
}

void on_Run_Pos6_toggled(GtkToggleButton *toggle_button)
{
    if (gtk_toggle_button_get_active(toggle_button))
    {
        g_print("Button 6 is ON \n");
    }
    else
    {
        g_print("Button 6 is OFF \n");
    }
}

void on_Run_Pos7_toggled(GtkToggleButton *toggle_button)
{
    if (gtk_toggle_button_get_active(toggle_button))
    {
        g_print("Button 7 is ON \n");
    }
    else
    {
        g_print("Button 7 is OFF \n");
    }
}
