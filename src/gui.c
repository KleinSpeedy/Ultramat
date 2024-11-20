/* This file contains the main implementation of the Graphical User Interface.
 * This consists of creating the different pages and implementing the main GUI thread.
*/

#include "callbacks_position.h"
#include "checks.h"
#include "gui.h"
#include "pages.h"

#include <gtk/gtk.h>

// Create all main window widgets in separate function with their attributes

static GtkWindow *create_main_window(void);
static GtkBox *create_main_box(void);
static GtkStack *create_main_window_stack(void);
static GtkStackSwitcher *create_main_stack_switcher(GtkStack *mainStack);
static GtkSwitch *create_main_motor_switch(void);
static void create_window_top_header(GtkStackSwitcher *mainStackSwitcher,
                                     GtkSwitch *mainMotorSwitch,
                                     GtkBox *mainBox);

// HACK: Find better solution than static var and getter
static GtkWindow *mainWindow_;
static GtkWindow *get_main_window(void)
{
    return mainWindow_;
}

void gui_thread(void)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "res/style.css", NULL);

    /* Create containers and navigation widgets */

    GtkWindow *mainWindow = create_main_window();
    GtkBox *mainBox = create_main_box();
    gtk_container_add(GTK_CONTAINER(mainWindow), GTK_WIDGET(mainBox));
    // TODO: Find better alternative
    mainWindow_ = mainWindow;

    GtkStack *mainStack = create_main_window_stack();
    gtk_box_pack_start(mainBox, GTK_WIDGET(mainStack), TRUE, TRUE, 0);
    GtkStackSwitcher *mainStackSwitcher = create_main_stack_switcher(mainStack);
    GtkSwitch *mainMotorSwitch = create_main_motor_switch();

    // create the main top header for navigation and motor control
    create_window_top_header(mainStackSwitcher, mainMotorSwitch, mainBox);
    // push header and navigational elements to top position
    gtk_box_reorder_child(mainBox, GTK_WIDGET(mainStack), 1);

    /* Stack pages */
    createStackPages(mainStack);

    /* apply style.css to window */
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(GTK_WIDGET(mainWindow));
    // Start GUI application
    gtk_main();
}

static GtkWindow *
create_main_window(void)
{
    /* Main window */
    GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    CHECK_WIDGET(window, "Main Window");

    /* Window Attributes and signals */
    gtk_window_set_title(window, "Ultramat Projekt");
    gtk_window_set_position(window, GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(window, FALSE);
    gtk_window_set_default_size(window, WIN_WIDTH, WIN_HEIGHT);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}

static GtkBox *
create_main_box(void)
{
    /* vertical orientation for header line and stack pages */
    GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    CHECK_WIDGET(box, "Main Box");

    return box;
}

static GtkStack *
create_main_window_stack(void)
{
    /* Stack holds different pages */
    GtkStack *stack  = GTK_STACK(gtk_stack_new());
    CHECK_WIDGET(stack, "Main Stack");

    gtk_stack_set_transition_type(stack, GTK_STACK_TRANSITION_TYPE_SLIDE_RIGHT);
    gtk_stack_set_transition_duration(stack, TRANSITION_SPEED);

    return stack;
}

static GtkStackSwitcher *
create_main_stack_switcher(GtkStack *mainStack)
{
    GtkStackSwitcher *stackSwitcher = GTK_STACK_SWITCHER(gtk_stack_switcher_new());
    CHECK_WIDGET(stackSwitcher, "Stack Switcher");
    gtk_stack_switcher_set_stack(stackSwitcher, mainStack);
    gtk_widget_set_name(GTK_WIDGET(stackSwitcher), "mainStackSwitcher");
    gtk_widget_set_size_request(GTK_WIDGET(stackSwitcher), 100, 100);

    return stackSwitcher;
}

static GtkSwitch *
create_main_motor_switch(void)
{
    GtkSwitch *motorSwitch = GTK_SWITCH(gtk_switch_new());
    CHECK_WIDGET(motorSwitch, "Motor Switch");
    gtk_widget_set_name(GTK_WIDGET(motorSwitch), "motorSwitch");
    gtk_widget_set_size_request(GTK_WIDGET(motorSwitch), 180, 100);

    // connect motor activate callback
    gulong id = g_signal_connect(motorSwitch, "state-set",
            G_CALLBACK(cb_on_motor_switch_toggle), NULL);
    cb_set_motor_switch_callback_id(id);

    return motorSwitch;
}
static void create_window_top_header(GtkStackSwitcher *mainStackSwitcher,
                                     GtkSwitch *mainMotorSwitch,
                                     GtkBox *mainBox)
{
    /* Fixed Container for placement */
    GtkFixed *headerFixed = GTK_FIXED(gtk_fixed_new());
    CHECK_WIDGET(headerFixed, "Header Layout");

    /* Project Header Label */
    GtkLabel *projectTitle = GTK_LABEL(gtk_label_new("Ultramat"));
    CHECK_WIDGET(projectTitle, "Project Title");
    // set name for css stylesheet
    gtk_widget_set_name(GTK_WIDGET(projectTitle), "projectTitle");

    gtk_fixed_put(headerFixed, GTK_WIDGET(mainStackSwitcher), 0, 0);
    gtk_fixed_put(headerFixed, GTK_WIDGET(projectTitle), 560, 25);
    gtk_fixed_put(headerFixed, GTK_WIDGET(mainMotorSwitch), 1100, 0);

    gtk_box_pack_start(mainBox, GTK_WIDGET(headerFixed), FALSE, FALSE, 0);
}

void
gui_show_error_modal(const gchar *errorStr)
{
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(get_main_window(),
                                    flags,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_CLOSE,
                                    "%s\n",
                                    errorStr);

    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}
