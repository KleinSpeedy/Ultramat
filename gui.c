#include "inc/gui.h"
#include "inc/pages.h"
#include "inc/drinks.h"
#include "inc/drinklists.h"

/* "private" function declaration */

// create the main Window and main vertical box
void createMainWindow();
// create the main stack that holds the different pages
void createWindowStack();
// create the header line with stack-switcher, title and motor switch
void createTopHeader();

/* Widgets and Variables */

GtkWindow *g_mainWindow;
GtkBox *g_mainBox;
GtkStack *g_mainStack;
GtkStackSwitcher *g_mainStackSwitcher;
GtkSwitch *g_mainMotorSwitch;

const int g_width = 1280;
const int g_height = 800;

/* function definitions */

void guiHandler(int argc, char **argv)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "./style.css", NULL);

    gtk_init(&argc, &argv);

    /* Create "broad" layout */ //TODO: remove g_vars and refactor
    createMainWindow();
    createWindowStack();
    createTopHeader();
    gtk_box_reorder_child(g_mainBox, GTK_WIDGET(g_mainStack), 1);

    Ing_Array_t *ingredientsArray = getAllIngredients();
    Rec_Array_t *recipeArray = getAllRecipes();

    GtkListStore *ingListStore = createIngredientListStore(ingredientsArray);
    GtkListStore *recListStore = createRecipeListStore(recipeArray);
    
    /* Stack pages */
    createStackPages(g_mainStack, ingListStore, recListStore);

    /* apply style.css to window */
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(GTK_WIDGET(g_mainWindow));

    gtk_main();

    free_ing_array(ingredientsArray);
    free_rec_array(recipeArray);
}

void createMainWindow()
{
    /* Main window */
    g_mainWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    CHECK_WIDGET(g_mainWindow, "Main Window");

    /* Window Attributes and signals */
    gtk_window_set_title(g_mainWindow, "Ultramat Projekt");
    gtk_window_set_resizable(g_mainWindow, FALSE);
    gtk_window_set_default_size(g_mainWindow, g_width, g_height);
    gtk_window_set_position(g_mainWindow, GTK_WIN_POS_CENTER);
    g_signal_connect(g_mainWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* vertical orientation for header line and stack pages */
    g_mainBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    CHECK_WIDGET(g_mainBox, "Main Box");

    gtk_container_add(GTK_CONTAINER(g_mainWindow), GTK_WIDGET(g_mainBox));
}

void createWindowStack()
{
    /* Stack holds different pages */
    g_mainStack = GTK_STACK(gtk_stack_new());
    CHECK_WIDGET(g_mainStack, "Main Stack");

    gtk_stack_set_transition_type(g_mainStack, GTK_STACK_TRANSITION_TYPE_SLIDE_RIGHT);
    gtk_stack_set_transition_duration(g_mainStack, 250 /*ms*/);

    gtk_box_pack_start(g_mainBox, GTK_WIDGET(g_mainStack), TRUE, TRUE, 0);
}

void createTopHeader()
{
    /* Fixed Container for placement */
    GtkFixed *headerFixed = GTK_FIXED(gtk_fixed_new());
    CHECK_WIDGET(headerFixed, "Header Layout");

    /* Stack Switcher */
    g_mainStackSwitcher = GTK_STACK_SWITCHER(gtk_stack_switcher_new());
    CHECK_WIDGET(g_mainStackSwitcher, "Stack Switcher");
    gtk_stack_switcher_set_stack(g_mainStackSwitcher, g_mainStack);
    gtk_widget_set_name(GTK_WIDGET(g_mainStackSwitcher), "mainStackSwitcher");
    gtk_widget_set_size_request(GTK_WIDGET(g_mainStackSwitcher), 100, 100);

    /* Project Header Label */
    GtkLabel *projectTitle = GTK_LABEL(gtk_label_new("Ultramat"));
    CHECK_WIDGET(projectTitle, "Project Title");
    // set name for css stylesheet
    gtk_widget_set_name(GTK_WIDGET(projectTitle), "projectTitle");

    /* Motor Enable Switch */
    g_mainMotorSwitch = GTK_SWITCH(gtk_switch_new());
    CHECK_WIDGET(g_mainMotorSwitch, "Motor Switch");
    gtk_widget_set_name(GTK_WIDGET(g_mainMotorSwitch), "motorSwitch");
    gtk_widget_set_size_request(GTK_WIDGET(g_mainMotorSwitch), 180, 100);

    gtk_fixed_put(headerFixed, GTK_WIDGET(g_mainStackSwitcher), 0, 0);
    gtk_fixed_put(headerFixed, GTK_WIDGET(projectTitle), 560, 25);
    gtk_fixed_put(headerFixed, GTK_WIDGET(g_mainMotorSwitch), 1100, 0);

    gtk_box_pack_start(g_mainBox, GTK_WIDGET(headerFixed), FALSE, FALSE, 0);
}
