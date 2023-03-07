/* This file populates the different stack pages. */

#include "checks.h"
#include "pages.h"
#include "callbacks.h"
#include "drinks.h"

/* "private" function declaration */

void populateStackPage_One(GtkStack *mainStack, DrinkManagement_t *dm);
void populateStackPage_Two(GtkStack *mainStack, DrinkManagement_t *dm);
void populateStackPage_Three(GtkStack *mainStack);
void populateStackPage_Four(GtkStack *mainStack);

/* function declaration */

void createStackPages(GtkStack *mainStack, DrinkManagement_t *dm)
{
    populateStackPage_One(mainStack, dm);
    populateStackPage_Two(mainStack, dm);
    populateStackPage_Three(mainStack);
}

/**
 * @brief Page one contains the positions with the selected ingredients
 * 
 * @param mainStack 
 * @param ingListStore 
 * @param ingArray
 */
void populateStackPage_One(GtkStack *mainStack, DrinkManagement_t *dm)
{
    GtkBox *pageOneBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    CHECK_WIDGET(pageOneBox, "Page 1 Box");

    /* let box fill parent container */
    gtk_widget_set_vexpand(GTK_WIDGET(pageOneBox), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(pageOneBox), TRUE);

    gtk_widget_set_name(GTK_WIDGET(pageOneBox), "Box_P_1"); // for debugging or design

    GtkGrid *selectionGrid = GTK_GRID(gtk_grid_new());
    CHECK_WIDGET(selectionGrid, "Drink Grid");

    gtk_grid_set_column_spacing(selectionGrid, 25);
    gtk_grid_set_row_spacing(selectionGrid, 50);

    /* 6 possible drinks, each consists of a Label, a Image and a Combo Box */

    GtkTreeModel *ingListModel = GTK_TREE_MODEL(dm->ingredientListStore);

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

    // register combo/entry callbacks
    g_signal_connect(comboPos1, "changed", G_CALLBACK(on_combo_pos1_changed), NULL);
    g_signal_connect(comboPos2, "changed", G_CALLBACK(on_combo_pos2_changed), ingListModel);

    // make combo boxes big enough, makes everything else bigger too
    int comboWidth = 200, comboHeight = 60;
    gtk_widget_set_size_request(GTK_WIDGET(comboPos1), comboWidth, comboHeight);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos2), comboWidth, comboHeight);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos3), comboWidth, comboHeight);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos4), comboWidth, comboHeight);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos5), comboWidth, comboHeight);
    gtk_widget_set_size_request(GTK_WIDGET(comboPos6), comboWidth, comboHeight);

    // make name column the one that gets displayed, id column the id column
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

/**
 * @brief Page 2 contains ordering a drink, with the progress shown in a progressbar
 * 
 * @param mainStack 
 * @param recListStore 
 */
void populateStackPage_Two(GtkStack *mainStack, DrinkManagement_t *dm)
{
    GtkBox *pageTwoBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    CHECK_WIDGET(pageTwoBox, "Page two box");
    /* let box fill parent container */
    gtk_widget_set_size_request(GTK_WIDGET(pageTwoBox), 1280, 700);
    
    gtk_widget_set_name(GTK_WIDGET(pageTwoBox), "Box_P_2"); // for debugging or design

    /* create separator for visual "appeal", align it in center */

    GtkSeparator *halfSeparator = GTK_SEPARATOR(gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    CHECK_WIDGET(halfSeparator, "Separator Page 2");
    
    /* set separator width and let it fill parent box vertically */
    gtk_widget_set_size_request(GTK_WIDGET(halfSeparator), 20, 0);
    gtk_widget_compute_expand(GTK_WIDGET(halfSeparator), GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_halign(GTK_WIDGET(halfSeparator), GTK_ALIGN_CENTER);

    /* Use left side of page 2 for ordering a drink, pack them in Box */

    GtkTreeModel *listModel = GTK_TREE_MODEL(dm->recipeListStore);

    GtkBox *boxComboOrder = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    GtkLabel *orderHeaderlabel = GTK_LABEL(gtk_label_new("Drink auswählen:"));
    GtkComboBox *comboOrder = GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(listModel));
    GtkToggleButton *orderStartButton = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Starte mixen!"));
    CHECK_WIDGET(boxComboOrder, "Box Page 2 links");
    CHECK_WIDGET(orderHeaderlabel, "Order Title");
    CHECK_WIDGET(comboOrder, "Order Combo Box");
    CHECK_WIDGET(orderStartButton, "Start order toggle button");

    gtk_combo_box_set_entry_text_column(comboOrder, 0);

    /* pack order widgets into page box and align properly */
    gtk_widget_set_size_request(GTK_WIDGET(comboOrder), 300, 70);
    gtk_box_pack_start(boxComboOrder, GTK_WIDGET(orderHeaderlabel), TRUE, FALSE, 0);
    gtk_box_pack_start(boxComboOrder, GTK_WIDGET(comboOrder), FALSE, FALSE, 0);
    gtk_box_pack_start(boxComboOrder, GTK_WIDGET(orderStartButton), FALSE, FALSE, 0);

    gtk_widget_set_valign(GTK_WIDGET(boxComboOrder), GTK_ALIGN_CENTER);

    /* get entry of combo box to set properties */
    GtkEntry *entryComboOrder = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboOrder)));
    gtk_entry_set_text(entryComboOrder, "Drink auswählen!");
    gtk_editable_set_editable(GTK_EDITABLE(entryComboOrder), FALSE);

    /* after mixing started show status in status-bar */

    GtkBox *progressbarBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    GtkProgressBar *progressbar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
    GtkLabel *progressbarLabel = GTK_LABEL(gtk_label_new("Status"));
    CHECK_WIDGET(progressbarBox, "Progressbar Box");
    CHECK_WIDGET(progressbar, "Progressbar");
    CHECK_WIDGET(progressbarLabel, "Progressbar label");

    /* pack progressbar and label into own box and align properly */
    gtk_box_pack_start(progressbarBox, GTK_WIDGET(progressbarLabel), FALSE, FALSE, 0);
    gtk_box_pack_start(progressbarBox, GTK_WIDGET(progressbar), FALSE, FALSE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(progressbar),300, 50);
    gtk_widget_set_name(GTK_WIDGET(progressbarLabel), "Progressbar_Label");

    gtk_widget_set_valign(GTK_WIDGET(progressbar), GTK_ALIGN_CENTER);

    // align progress bar box
    gtk_widget_set_valign(GTK_WIDGET(progressbarBox), GTK_ALIGN_CENTER);

    /* pack everything into page box */

    gtk_box_pack_start(pageTwoBox, GTK_WIDGET(boxComboOrder), TRUE, FALSE, 0);
    gtk_box_pack_start(pageTwoBox, GTK_WIDGET(halfSeparator), TRUE, FALSE, 0);
    gtk_box_pack_start(pageTwoBox, GTK_WIDGET(progressbarBox), TRUE, FALSE, 0); 

    /* make sure page two box is centered (slightly unnecessary -> size request ?) */
    gtk_widget_set_halign(GTK_WIDGET(pageTwoBox), GTK_ALIGN_CENTER);

    gtk_stack_add_titled(mainStack, GTK_WIDGET(pageTwoBox), "Page_Two", "Bestellung");
}

void populateStackPage_Three(GtkStack *mainStack)
{
    GtkBox *pageThreeBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    CHECK_WIDGET(pageThreeBox, "Page 3 Box");
    
    /* let box fill parent container */
    gtk_widget_set_size_request(GTK_WIDGET(pageThreeBox), 1280, 700);
    
    GtkBox *manualPosBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 25));
    CHECK_WIDGET(manualPosBox, "Manual buttons grid");

    /* align buttons-box vertically */
    gtk_widget_set_valign(GTK_WIDGET(manualPosBox), GTK_ALIGN_CENTER);

    GtkToggleButton *manualPos1 = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("1"));
    GtkToggleButton *manualPos2 = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("2"));
    GtkToggleButton *manualPos3 = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("3"));
    GtkToggleButton *manualPos4 = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("4"));
    GtkToggleButton *manualPos5 = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("5"));
    GtkToggleButton *manualPos6 = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("6"));

    int buttonWidth = 50, buttonHeight = 50, buttonPadding = 10;

    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos1), TRUE, TRUE, buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos2), TRUE, TRUE, buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos3), TRUE, TRUE, buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos4), TRUE, TRUE, buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos5), TRUE, TRUE, buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos6), TRUE, TRUE, buttonPadding);

    gtk_box_pack_start(pageThreeBox, GTK_WIDGET(manualPosBox), TRUE, TRUE, 25);

    gtk_stack_add_titled(mainStack, GTK_WIDGET(pageThreeBox), "Page_Three", "Manuell");
}

void populateStackPage_Four(GtkStack *mainStack)
{

}