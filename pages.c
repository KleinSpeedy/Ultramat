#include "inc/pages.h"

/* "private" function declaration */

void populateStackPage_One();

/* variables */

GtkStack *mainStack;
GtkListStore *g_ingListStore;
GtkListStore *g_recListStore;

/* function declaration */

void createStackPages(GtkStack *g_mainStack, GtkListStore *ingListStore)
{
    mainStack = g_mainStack;
    populateStackPage_One(ingListStore);
}

void getListStores(GtkListStore *ingListStore, GtkListStore *recListStore)
{
    g_ingListStore = ingListStore;
    g_recListStore = recListStore;
}

void populateStackPage_One(GtkListStore *ingListStore)
{
    GtkBox *pageOneBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    CHECK_WIDGET(pageOneBox, "Page 1 Box");

    GtkGrid *selectionGrid = GTK_GRID(gtk_grid_new());
    CHECK_WIDGET(selectionGrid, "Drink Grid");
    
    gtk_grid_set_column_spacing(selectionGrid, 25);
    gtk_grid_set_row_spacing(selectionGrid, 25);

    /* 6 possible drinks, each consists of a Label, a Image and a Combo Box */

    GtkTreeModel *ingListModel = GTK_TREE_MODEL(ingListStore);

    GtkBox *boxPos1 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkImage *imagePos1 = GTK_IMAGE(gtk_image_new());
    GtkLabel *labelPos1 = GTK_LABEL(gtk_label_new("Position 1"));
    GtkComboBox *comboPos1 = GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(ingListModel));
    CHECK_WIDGET(boxPos1, "Pos 1");
    CHECK_WIDGET(labelPos1, "Label Pos 1");
    CHECK_WIDGET(imagePos1, "Image Pos 1");
    CHECK_WIDGET(comboPos1, "Combo Pos 1");

    GtkBox *boxPos2 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos2 = GTK_LABEL(gtk_label_new("Position 2"));
    GtkImage *imagePos2 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos2 = GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(ingListModel));
    CHECK_WIDGET(boxPos2, "Pos 2");
    CHECK_WIDGET(labelPos2, "Label Pos 2");
    CHECK_WIDGET(imagePos2, "Image Pos 2");
    CHECK_WIDGET(comboPos2, "Combo Pos 2");

    GtkBox *boxPos3 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos3 = GTK_LABEL(gtk_label_new("Position 3"));
    GtkImage *imagePos3 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos3 = GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(ingListModel));
    CHECK_WIDGET(boxPos3, "Pos 3");
    CHECK_WIDGET(labelPos3, "Label Pos 3");
    CHECK_WIDGET(imagePos3, "Image Pos 3");
    CHECK_WIDGET(comboPos3, "Combo Pos 3");

    GtkBox *boxPos4 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos4 = GTK_LABEL(gtk_label_new("Position 4"));
    GtkImage *imagePos4 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos4 = GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(ingListModel));
    CHECK_WIDGET(boxPos4, "Pos 4");
    CHECK_WIDGET(labelPos4, "Label Pos 4");
    CHECK_WIDGET(imagePos4, "Image Pos 4");
    CHECK_WIDGET(comboPos4, "Combo Pos 4");

    GtkBox *boxPos5 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos5 = GTK_LABEL(gtk_label_new("Position 5"));
    GtkImage *imagePos5 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos5 = GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(ingListModel));
    CHECK_WIDGET(boxPos5, "Pos 5");
    CHECK_WIDGET(labelPos5, "Label Pos 5");
    CHECK_WIDGET(imagePos5, "Image Pos 5");
    CHECK_WIDGET(comboPos5, "Combo Pos 5");

    GtkBox *boxPos6 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkLabel *labelPos6 = GTK_LABEL(gtk_label_new("Position 6"));
    GtkImage *imagePos6 = GTK_IMAGE(gtk_image_new());
    GtkComboBox *comboPos6 = GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(ingListModel));
    CHECK_WIDGET(boxPos6, "Pos 6");
    CHECK_WIDGET(labelPos6, "Label Pos 6");
    CHECK_WIDGET(imagePos6, "Image Pos 6");
    CHECK_WIDGET(comboPos6, "Combo Pos 6");

    // extract entries from combo boxes, display default text and disable editable
    GtkEntry *entryPos1 = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboPos1)));
    GtkEntry *entryPos2 = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboPos2)));
    GtkEntry *entryPos3 = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboPos3)));
    GtkEntry *entryPos4 = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboPos4)));
    GtkEntry *entryPos5 = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboPos5)));
    GtkEntry *entryPos6 = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboPos6)));

    gtk_entry_set_text(entryPos1, "Zutat auswählen!");
    gtk_entry_set_text(entryPos2, "Zutat auswählen!");
    gtk_entry_set_text(entryPos3, "Zutat auswählen!");
    gtk_entry_set_text(entryPos4, "Zutat auswählen!");
    gtk_entry_set_text(entryPos5, "Zutat auswählen!");
    gtk_entry_set_text(entryPos6, "Zutat auswählen!");

    gtk_editable_set_editable(GTK_EDITABLE(entryPos1), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entryPos2), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entryPos3), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entryPos4), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entryPos5), FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entryPos6), FALSE);    

    // make combo boxes big enough, makes everything else bigger too
    gtk_widget_set_size_request(GTK_WIDGET(comboPos1), 200, 50);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos2), 200, 50);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos3), 200, 50);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos4), 200, 50);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos5), 200, 50);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos6), 200, 50);

    // make name column the one that gets displayed
    gtk_combo_box_set_entry_text_column(comboPos1, 0);
    gtk_combo_box_set_entry_text_column(comboPos2, 0);
    gtk_combo_box_set_entry_text_column(comboPos3, 0);
    gtk_combo_box_set_entry_text_column(comboPos4, 0);
    gtk_combo_box_set_entry_text_column(comboPos5, 0);
    gtk_combo_box_set_entry_text_column(comboPos6, 0);

    // pack every widget into according box
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

    // move grid to horizontal center, vertical center achieved through padding of box
    gtk_widget_set_halign(GTK_WIDGET(selectionGrid), GTK_ALIGN_CENTER);
    gtk_box_pack_start(pageOneBox, GTK_WIDGET(selectionGrid), TRUE, TRUE, 150);

    gtk_stack_add_titled(mainStack, GTK_WIDGET(pageOneBox), "Page_One", "Positionen");
}