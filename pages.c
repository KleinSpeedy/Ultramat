#include "inc/pages.h"

/* "private" function declaration */

void populateStackPage_One();

/* variables */

GtkStack *mainStack;

/* function declaration */

void createStackPages(GtkStack *g_mainStack)
{
    mainStack = g_mainStack;
    populateStackPage_One();
}

void populateStackPage_One()
{
    GtkGrid *selectionGrid = GTK_GRID(gtk_grid_new());
    CHECK_WIDGET(selectionGrid, "Drink Grid");
    
    /* 6 possible drinks, each consists of a Label, a Image and a Combo Box */

    GtkBox *boxPos1 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkImage *imagePos1 = GTK_IMAGE(gtk_image_new());
    GtkLabel *labelPos1 = GTK_LABEL(gtk_label_new("Position 1"));
    GtkComboBox *comboPos1 = GTK_COMBO_BOX(gtk_combo_box_text_new());
    CHECK_WIDGET(boxPos1, "Pos 1");
    CHECK_WIDGET(labelPos1, "Label Pos 1");
    CHECK_WIDGET(imagePos1, "Image Pos 1");
    CHECK_WIDGET(comboPos1, "Combo Pos 1");

    GtkBox *boxPos2 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos2 = GTK_LABEL(gtk_label_new("Position 2"));
    GtkImage *imagePos2 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos2 = GTK_COMBO_BOX(gtk_combo_box_text_new());
    CHECK_WIDGET(boxPos2, "Pos 2");
    CHECK_WIDGET(labelPos2, "Label Pos 2");
    CHECK_WIDGET(imagePos2, "Image Pos 2");
    CHECK_WIDGET(comboPos2, "Combo Pos 2");

    GtkBox *boxPos3 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos3 = GTK_LABEL(gtk_label_new("Position 3"));
    GtkImage *imagePos3 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos3 = GTK_COMBO_BOX(gtk_combo_box_text_new());
    CHECK_WIDGET(boxPos3, "Pos 3");
    CHECK_WIDGET(labelPos3, "Label Pos 3");
    CHECK_WIDGET(imagePos3, "Image Pos 3");
    CHECK_WIDGET(comboPos3, "Combo Pos 3");

    GtkBox *boxPos4 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos4 = GTK_LABEL(gtk_label_new("Position 4"));
    GtkImage *imagePos4 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos4 = GTK_COMBO_BOX(gtk_combo_box_text_new());
    CHECK_WIDGET(boxPos4, "Pos 4");
    CHECK_WIDGET(labelPos4, "Label Pos 4");
    CHECK_WIDGET(imagePos4, "Image Pos 4");
    CHECK_WIDGET(comboPos4, "Combo Pos 4");

    GtkBox *boxPos5 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos5 = GTK_LABEL(gtk_label_new("Position 5"));
    GtkImage *imagePos5 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos5 = GTK_COMBO_BOX(gtk_combo_box_text_new());
    CHECK_WIDGET(boxPos5, "Pos 5");
    CHECK_WIDGET(labelPos5, "Label Pos 5");
    CHECK_WIDGET(imagePos5, "Image Pos 5");
    CHECK_WIDGET(comboPos5, "Combo Pos 5");

    GtkBox *boxPos6 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos6 = GTK_LABEL(gtk_label_new("Position 6"));
    GtkImage *imagePos6 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos6 = GTK_COMBO_BOX(gtk_combo_box_text_new());
    CHECK_WIDGET(boxPos6, "Pos 6");
    CHECK_WIDGET(labelPos6, "Label Pos 6");
    CHECK_WIDGET(imagePos6, "Image Pos 6");
    CHECK_WIDGET(comboPos6, "Combo Pos 6");

    gtk_box_pack_start(boxPos1, GTK_WIDGET(labelPos1), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos1, GTK_WIDGET(imagePos1), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos1, GTK_WIDGET(comboPos1), TRUE, TRUE, 0);
    gtk_grid_attach(selectionGrid, GTK_WIDGET(boxPos1), 0, 0, 1, 1);

    gtk_box_pack_start(boxPos2, GTK_WIDGET(labelPos2), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos2, GTK_WIDGET(imagePos2), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos2, GTK_WIDGET(comboPos2), TRUE, TRUE, 0);
    gtk_grid_attach(selectionGrid, GTK_WIDGET(boxPos2), 1, 0, 1, 1);

    gtk_box_pack_start(boxPos3, GTK_WIDGET(labelPos3), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos3, GTK_WIDGET(imagePos3), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos3, GTK_WIDGET(comboPos3), TRUE, TRUE, 0);
    gtk_grid_attach(selectionGrid, GTK_WIDGET(boxPos3), 2, 0, 1, 1);

    gtk_box_pack_start(boxPos4, GTK_WIDGET(labelPos4), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos4, GTK_WIDGET(imagePos4), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos4, GTK_WIDGET(comboPos4), TRUE, TRUE, 0);
    gtk_grid_attach(selectionGrid, GTK_WIDGET(boxPos4), 0, 1, 1, 1);

    gtk_box_pack_start(boxPos5, GTK_WIDGET(labelPos5), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos5, GTK_WIDGET(imagePos5), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos5, GTK_WIDGET(comboPos5), TRUE, TRUE, 0);
    gtk_grid_attach(selectionGrid, GTK_WIDGET(boxPos5), 1, 1, 1, 1);

    gtk_box_pack_start(boxPos6, GTK_WIDGET(labelPos6), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos6, GTK_WIDGET(imagePos6), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos6, GTK_WIDGET(comboPos6), TRUE, TRUE, 0);
    gtk_grid_attach(selectionGrid, GTK_WIDGET(boxPos6), 2, 1, 1, 1);

    gtk_stack_add_titled(mainStack, GTK_WIDGET(selectionGrid), "Selection Grid", "Positionen");
}