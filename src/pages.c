/* This file populates the different stack pages. */

#include "pages.h"
#include "callbacks_info.h"
#include "callbacks_position.h"
#include "checks.h"
#include "drinklists.h"
#include "glib-object.h"
#include "gui.h"
#include "proto/commands.pb.h"
#include "ultra_version.h"

#include <glib.h>
#include <gtk/gtk.h>

// make combo boxes big enough, makes everything else bigger too
#define COMBO_WIDTH 200
#define COMBO_HEIGHT 50

// page one grid spacing between rows and columns
#define GRID_SPACING 25

// page one grid helper struct
typedef struct grid_pos
{
    const int col, row;
} GridPos_t;

/* "private" function declaration */

static void populateStackPage_One(GtkStack *mainStack);
static void populateStackPage_Two(GtkStack *mainStack);
static void populateStackPage_Three(GtkStack *mainStack);
static void populateStackPage_Four(GtkStack *mainStack);

/* function declaration */

void createStackPages(GtkStack *mainStack)
{
    populateStackPage_One(mainStack);
    populateStackPage_Two(mainStack);
    populateStackPage_Three(mainStack);
    populateStackPage_Four(mainStack);
}

static GtkBox *add_ing_selection(const IngPos_t *ingPos)
{
    GtkTreeModel *ingListModel = GTK_TREE_MODEL(lists_ingredient_store());
    const gchar *posStr = cb_get_combo_pos_string(*ingPos);

    GtkBox *boxPos = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    GtkBox *boxComboReset = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));

    GtkImage *imagePos = GTK_IMAGE(gtk_image_new());
    GtkLabel *labelPos = GTK_LABEL(gtk_label_new(posStr));
    GtkComboBox *comboPos =
        GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(ingListModel));
    GtkButton *btnResetPos = GTK_BUTTON(
        gtk_button_new_from_icon_name("edit-delete", GTK_ICON_SIZE_DND));

    // set image widget and reset inital image
    cb_set_combo_position_image(imagePos, *ingPos);

    // extract entries from combo boxes, display default text and disable
    // editable
    GtkEntry *entryPos = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboPos)));
    gtk_entry_set_text(entryPos, RESET_TEXT);
    gtk_editable_set_editable(GTK_EDITABLE(entryPos), FALSE);

    // connect callback handler IDs
    g_signal_connect(btnResetPos, "clicked",
                     G_CALLBACK(cb_on_reset_btn_clicked), (gpointer)ingPos);

    // TODO: Pos one is reserved for pressure pumps
    const gulong id = g_signal_connect(comboPos, "changed",
                                       G_CALLBACK(cb_on_combo_position_changed),
                                       (gpointer)ingPos);
    cb_set_combo_position_callback(comboPos, *ingPos, id);

    gtk_widget_set_size_request(GTK_WIDGET(comboPos), COMBO_WIDTH,
                                COMBO_HEIGHT);

    // make name column the one that gets displayed
    gtk_combo_box_set_entry_text_column(comboPos, ING_COLUMN_NAME);

    // pack every widget into according box
    gtk_box_pack_start(boxComboReset, GTK_WIDGET(btnResetPos), TRUE, TRUE, 0);
    gtk_box_pack_start(boxComboReset, GTK_WIDGET(comboPos), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos, GTK_WIDGET(labelPos), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos, GTK_WIDGET(imagePos), TRUE, TRUE, 0);
    gtk_box_pack_start(boxPos, GTK_WIDGET(boxComboReset), TRUE, TRUE, 0);

    return boxPos;
}

// populate stack page one with widgets for ingredient selection
static void populateStackPage_One(GtkStack *mainStack)
{
    static const GridPos_t gridPositions[MAX_INGREDIENT_POSITIONS] = {
        {0, 0}, {1, 0}, {2, 0}, // < "pos one" pressure pump 1 - 3
        {0, 1}, {1, 1}, {2, 1}, // < positions 1 - 3
        {0, 2}, {1, 2}, {2, 2}  // < positions 4 - 6
    };

    static const IngPos_t selectPositions[MAX_INGREDIENT_POSITIONS] = {
        {PAGES_COMBO_POS_ONE, PUMP_POS_ONE},
        {PAGES_COMBO_POS_ONE, PUMP_POS_TWO},
        {PAGES_COMBO_POS_ONE, PUMP_POS_THREE},
        {PAGES_COMBO_POS_TWO, PUMP_POS_INVALID},
        {PAGES_COMBO_POS_THREE, PUMP_POS_INVALID},
        {PAGES_COMBO_POS_FOUR, PUMP_POS_INVALID},
        {PAGES_COMBO_POS_FIVE, PUMP_POS_INVALID},
        {PAGES_COMBO_POS_SIX, PUMP_POS_INVALID},
        {PAGES_COMBO_POS_SEVEN, PUMP_POS_INVALID},
    };

    GtkBox *pageOneBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));

    /* let box fill parent container */
    gtk_widget_set_vexpand(GTK_WIDGET(pageOneBox), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(pageOneBox), TRUE);

    gtk_widget_set_name(GTK_WIDGET(pageOneBox),
                        "Box_P_1"); // for debugging or design

    GtkGrid *selectionGrid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_column_spacing(selectionGrid, GRID_SPACING);
    gtk_grid_set_row_spacing(selectionGrid, GRID_SPACING);

    for(int i = 0; i < MAX_INGREDIENT_POSITIONS; i++)
    {
        GtkBox *boxPos = add_ing_selection(&selectPositions[i]);
        const GridPos_t gridPos = gridPositions[i];

        gtk_grid_attach(selectionGrid, GTK_WIDGET(boxPos), gridPos.col,
                        gridPos.row, 1, 1);
    }

    // move grid to horizontal center, vertical center achieved through padding
    // of box
    gtk_widget_set_halign(GTK_WIDGET(selectionGrid), GTK_ALIGN_CENTER);
    gtk_box_pack_start(pageOneBox, GTK_WIDGET(selectionGrid), TRUE, TRUE, 10);

    gtk_stack_add_titled(mainStack, GTK_WIDGET(pageOneBox), "Page_One",
                         "Positionen");
}

/**
 * @brief Page 2 contains ordering a drink, with the progress shown in a
 * progressbar
 *
 * @param mainStack
 * @param recListStore
 */
void populateStackPage_Two(GtkStack *mainStack)
{
    GtkBox *pageTwoBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    CHECK_WIDGET(pageTwoBox, "Page two box");

    /* let box fill parent container */
    gtk_widget_set_vexpand(GTK_WIDGET(pageTwoBox), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(pageTwoBox), TRUE);

    gtk_widget_set_name(GTK_WIDGET(pageTwoBox),
                        "Box_P_2"); // for debugging or design

    /* create separator for visual "appeal", align it in center */

    GtkSeparator *halfSeparator =
        GTK_SEPARATOR(gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    CHECK_WIDGET(halfSeparator, "Separator Page 2");

    /* set separator width and let it fill parent box vertically */
    gtk_widget_set_size_request(GTK_WIDGET(halfSeparator), 10, 0);
    gtk_widget_compute_expand(GTK_WIDGET(halfSeparator),
                              GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_halign(GTK_WIDGET(halfSeparator), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(GTK_WIDGET(halfSeparator), 50);
    gtk_widget_set_margin_end(GTK_WIDGET(halfSeparator), 50);

    /* Use left side of page 2 for ordering a drink, pack them in Box */

    GtkTreeModel *listModel = GTK_TREE_MODEL(lists_recipe_store());

    GtkBox *boxComboOrder = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    GtkLabel *orderHeaderlabel = GTK_LABEL(gtk_label_new("Drink auswählen:"));
    GtkComboBox *comboOrder =
        GTK_COMBO_BOX(gtk_combo_box_new_with_model_and_entry(listModel));
    GtkToggleButton *orderStartButton =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Starte mixen!"));
    CHECK_WIDGET(boxComboOrder, "Box Page 2 links");
    CHECK_WIDGET(orderHeaderlabel, "Order Title");
    CHECK_WIDGET(comboOrder, "Order Combo Box");
    CHECK_WIDGET(orderStartButton, "Start order toggle button");

    gtk_combo_box_set_entry_text_column(comboOrder, REC_COLUMN_NAME);

    /* pack order widgets into page box and align properly */
    gtk_widget_set_size_request(GTK_WIDGET(comboOrder), 400, 70);
    gtk_box_pack_start(boxComboOrder, GTK_WIDGET(orderHeaderlabel), TRUE, FALSE,
                       0);
    gtk_box_pack_start(boxComboOrder, GTK_WIDGET(comboOrder), FALSE, FALSE, 0);
    gtk_box_pack_start(boxComboOrder, GTK_WIDGET(orderStartButton), FALSE,
                       FALSE, 0);

    gtk_widget_set_valign(GTK_WIDGET(boxComboOrder), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(boxComboOrder), GTK_ALIGN_START);
    // push 140 pixels from left window border
    gtk_widget_set_margin_start(GTK_WIDGET(boxComboOrder), 140);
    gtk_widget_set_margin_end(GTK_WIDGET(boxComboOrder), 0);

    /* get entry of combo box to set properties */
    GtkEntry *entryComboOrder =
        GTK_ENTRY(gtk_bin_get_child(GTK_BIN(comboOrder)));
    gtk_entry_set_text(entryComboOrder, "Drink auswählen!");
    gtk_editable_set_editable(GTK_EDITABLE(entryComboOrder), FALSE);

    /* activate order button once serial communication is established */
    gtk_widget_set_sensitive(GTK_WIDGET(orderStartButton), FALSE);

    /* Connect order-recipe combo box with callback */
    gulong id = g_signal_connect(comboOrder, "changed",
                                 G_CALLBACK(cb_on_combo_order_changed), NULL);
    cb_set_combo_order_callback_id(id);
    cb_set_combo_order_widget(comboOrder);

    id = g_signal_connect(orderStartButton, "toggled",
                          G_CALLBACK(cb_on_recipe_order_toggle), NULL);
    cb_set_button_order_callback_id(id);
    cb_set_button_order_widget(orderStartButton);

    /* after mixing started show status in status-bar */

    GtkBox *progressbarBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    GtkProgressBar *progressbar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
    GtkLabel *progressbarLabel = GTK_LABEL(gtk_label_new("Status"));
    CHECK_WIDGET(progressbarBox, "Progressbar Box");
    CHECK_WIDGET(progressbar, "Progressbar");
    CHECK_WIDGET(progressbarLabel, "Progressbar label");

    gtk_box_pack_start(progressbarBox, GTK_WIDGET(progressbarLabel), TRUE,
                       FALSE, 0);
    gtk_box_pack_start(progressbarBox, GTK_WIDGET(progressbar), TRUE, FALSE, 0);

    /* set progressbar height and let it fill parent box horizontally */
    gtk_widget_set_size_request(GTK_WIDGET(progressbar), 400, 0);
    gtk_widget_compute_expand(GTK_WIDGET(progressbar),
                              GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_valign(GTK_WIDGET(progressbar), GTK_ALIGN_CENTER);

    gtk_widget_set_name(GTK_WIDGET(progressbarLabel), "Progressbar_Label");

    // align progress bar box
    gtk_widget_set_valign(GTK_WIDGET(progressbarBox), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(progressbarBox), GTK_ALIGN_END);
    // push 140 pixels from right window border
    gtk_widget_set_margin_start(GTK_WIDGET(progressbarBox), 0);
    gtk_widget_set_margin_end(GTK_WIDGET(progressbarBox), 140);

    cb_set_progress_bar_widget(progressbar);

    /* pack everything into page box */

    gtk_box_pack_start(pageTwoBox, GTK_WIDGET(boxComboOrder), TRUE, FALSE, 0);
    gtk_box_pack_start(pageTwoBox, GTK_WIDGET(halfSeparator), TRUE, FALSE, 0);
    gtk_box_pack_start(pageTwoBox, GTK_WIDGET(progressbarBox), TRUE, FALSE, 0);

    /* make sure page two box is centered (slightly unnecessary -> size request
     * ?) */
    // gtk_widget_set_halign(GTK_WIDGET(pageTwoBox), GTK_ALIGN_CENTER);

    gtk_stack_add_titled(mainStack, GTK_WIDGET(pageTwoBox), "Page_Two",
                         "Bestellung");
}

void populateStackPage_Three(GtkStack *mainStack)
{
    GtkBox *pageThreeBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    CHECK_WIDGET(pageThreeBox, "Page 3 Box");

    /* let box fill parent container */
    gtk_widget_set_vexpand(GTK_WIDGET(pageThreeBox), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(pageThreeBox), TRUE);

    GtkBox *manualPosBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 25));
    CHECK_WIDGET(manualPosBox, "Manual buttons grid");

    /* align buttons-box vertically */
    gtk_widget_set_valign(GTK_WIDGET(manualPosBox), GTK_ALIGN_CENTER);

    GtkToggleButton *manualPos1 =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Pumpen"));
    GtkToggleButton *manualPos2 =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Position 1"));
    GtkToggleButton *manualPos3 =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Position 2"));
    GtkToggleButton *manualPos4 =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Position 3"));
    GtkToggleButton *manualPos5 =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Position 4"));
    GtkToggleButton *manualPos6 =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Position 5"));
    GtkToggleButton *manualPos7 =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Position 6"));
    GtkToggleButton *manualPosFinal =
        GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label("Ausgabe"));

    const gint buttonPadding = 10;

    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos1), TRUE, TRUE,
                       buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos2), TRUE, TRUE,
                       buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos3), TRUE, TRUE,
                       buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos4), TRUE, TRUE,
                       buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos5), TRUE, TRUE,
                       buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos6), TRUE, TRUE,
                       buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPos7), TRUE, TRUE,
                       buttonPadding);
    gtk_box_pack_start(manualPosBox, GTK_WIDGET(manualPosFinal), TRUE, TRUE,
                       buttonPadding);

    GtkBox *yMoveBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 25));
    CHECK_WIDGET(yMoveBox, "Manual y move");

    GtkToggleButton *yMoveBtn = GTK_TOGGLE_BUTTON(
        gtk_toggle_button_new_with_label("Vertikale Bewegung"));

    gtk_box_pack_start(yMoveBox, GTK_WIDGET(yMoveBtn), TRUE, FALSE,
                       buttonPadding);

    // Set callback handlers and ids for x and y manual buttons

    // y button
    gulong id = g_signal_connect(yMoveBtn, "toggled",
                                 G_CALLBACK(cb_on_manual_pos_y_toggle), NULL);
    cb_set_manual_pos_y_callback(yMoveBtn, id);

    // x buttons
    // position 1
    id = g_signal_connect(manualPos1, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_ONE));
    cb_set_manual_pos_callback(manualPos1, PositionX_ONE, id);
    // position 2
    id = g_signal_connect(manualPos2, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_TWO));
    cb_set_manual_pos_callback(manualPos2, PositionX_TWO, id);
    // position 3
    id = g_signal_connect(manualPos3, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_THREE));
    cb_set_manual_pos_callback(manualPos3, PositionX_THREE, id);
    // position 4
    id = g_signal_connect(manualPos4, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_FOUR));
    cb_set_manual_pos_callback(manualPos4, PositionX_FOUR, id);
    // position 5
    id = g_signal_connect(manualPos5, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_FIVE));
    cb_set_manual_pos_callback(manualPos5, PositionX_FIVE, id);
    // position 6
    id = g_signal_connect(manualPos6, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_SIX));
    cb_set_manual_pos_callback(manualPos6, PositionX_SIX, id);
    // position 7
    id = g_signal_connect(manualPos7, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_SEVEN));
    cb_set_manual_pos_callback(manualPos7, PositionX_SEVEN, id);
    // position final
    id = g_signal_connect(manualPosFinal, "toggled",
                          G_CALLBACK(cb_on_manual_pos_toggle),
                          GINT_TO_POINTER(PositionX_FINAL));
    cb_set_manual_pos_callback(manualPosFinal, PositionX_FINAL, id);

    gtk_box_pack_start(pageThreeBox, GTK_WIDGET(manualPosBox), TRUE, TRUE, 25);
    gtk_box_pack_start(pageThreeBox, GTK_WIDGET(yMoveBox), TRUE, TRUE, 25);
    gtk_stack_add_titled(mainStack, GTK_WIDGET(pageThreeBox), "Page_Three",
                         "Manuell");
}

void populateStackPage_Four(GtkStack *mainStack)
{
    GtkBox *pageFourBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    CHECK_WIDGET(pageFourBox, "Page 4 Box");

    /* let box fill parent container */
    gtk_widget_set_vexpand(GTK_WIDGET(pageFourBox), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(pageFourBox), TRUE);

    GtkGrid *versionGrid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_row_spacing(versionGrid, 25);
    gtk_grid_set_column_spacing(versionGrid, 25);
    gtk_widget_set_halign(GTK_WIDGET(versionGrid), GTK_ALIGN_CENTER);

    /* App version informations */
    GtkLabel *appInfoLabel = GTK_LABEL(gtk_label_new("App version:"));
    GtkLabel *appVersion = GTK_LABEL(gtk_label_new(ULTRA_VERSION));
    GtkButton *appUpdateButton =
        GTK_BUTTON(gtk_button_new_with_label("Check Update"));
    gtk_widget_set_size_request(GTK_WIDGET(appUpdateButton), 60, 100);
    g_signal_connect(appUpdateButton, "clicked",
                     G_CALLBACK(cb_info_app_update_button_clicked), NULL);

    gtk_grid_attach(versionGrid, GTK_WIDGET(appInfoLabel), 0, 0, 1, 1);
    gtk_grid_attach_next_to(versionGrid, GTK_WIDGET(appVersion),
                            GTK_WIDGET(appInfoLabel), GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to(versionGrid, GTK_WIDGET(appUpdateButton),
                            GTK_WIDGET(appVersion), GTK_POS_RIGHT, 1, 1);

    /* Microcontroller version informations */
    GtkLabel *mcInfoLabel =
        GTK_LABEL(gtk_label_new("Microcontroller version:"));
    GtkLabel *mcVersion = GTK_LABEL(gtk_label_new("Nicht ausgelesen"));
    GtkButton *mcUpdateButton =
        GTK_BUTTON(gtk_button_new_with_label("Check Update"));
    gtk_widget_set_size_request(GTK_WIDGET(mcUpdateButton), 60, 100);

    cb_info_set_mc_version_label(mcVersion);
    g_signal_connect(mcUpdateButton, "clicked",
                     G_CALLBACK(cb_info_mc_update_button_clicked), NULL);

    gtk_grid_attach(versionGrid, GTK_WIDGET(mcInfoLabel), 0, 1, 1, 1);
    gtk_grid_attach_next_to(versionGrid, GTK_WIDGET(mcVersion),
                            GTK_WIDGET(mcInfoLabel), GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to(versionGrid, GTK_WIDGET(mcUpdateButton),
                            GTK_WIDGET(mcVersion), GTK_POS_RIGHT, 1, 1);

    gtk_label_set_xalign(GTK_LABEL(appInfoLabel), 1.0); // right-align
    gtk_label_set_xalign(GTK_LABEL(mcInfoLabel), 1.0);  // right-align

    // utility quit button
    // TODO: Good idea??
    GtkButton *quitButton = GTK_BUTTON(gtk_button_new_from_icon_name(
        "application-exit", GTK_ICON_SIZE_DIALOG));
    gtk_widget_set_size_request(GTK_WIDGET(quitButton), 60, 100);
    g_signal_connect(quitButton, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_grid_attach(versionGrid, GTK_WIDGET(quitButton), 0, 2, 1, 1);

    /* attach version grid to box */
    gtk_box_pack_start(pageFourBox, GTK_WIDGET(versionGrid), FALSE, FALSE, 25);

    gtk_stack_add_titled(mainStack, GTK_WIDGET(pageFourBox), "Page_Four",
                         "Info");
}
