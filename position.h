#ifndef POSITION_H
#define POSITION_H

/* -- INCLUDES -- */

#include <gtk/gtk.h>

/* -- METHODS -- */

// Setup of Stack-Page4
void setupPage4(GtkBuilder *builder);

// Signal Handler Toggle-Button-1
void on_Run_Pos1_toggled(GtkToggleButton *toggle_button);
// Signal Handler Toggle-Button-2
void on_Run_Pos2_toggled(GtkToggleButton *toggle_button);
// Signal Handler Toggle-Button-3
void on_Run_Pos3_toggled(GtkToggleButton *toggle_button);
// Signal Handler Toggle-Button-4
void on_Run_Pos4_toggled(GtkToggleButton *toggle_button);
// Signal Handler Toggle-Button-5
void on_Run_Pos5_toggled(GtkToggleButton *toggle_button);
// Signal Handler Toggle-Button-6
void on_Run_Pos6_toggled(GtkToggleButton *toggle_button);
// Signal Handler Toggle-Button-7
void on_Run_Pos7_toggled(GtkToggleButton *toggle_button);

/* -- VARIABLES -- */
// Are Stepper Motors active or not?
uint8_t STEPPER_ACTIVE = 0;
    
#endif //POSITION_H