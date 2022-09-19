#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>

#include "position.h"

void css_set(GtkCssProvider *prov, GtkWidget *widg);

// Make them global

GtkBuilder *builder;
GtkWidget *window;

int main(int argc, char **argv)
{

    GError *error = NULL;
    GtkCssProvider *css_provider;

    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "./style.css", NULL);

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();

    if (gtk_builder_add_from_file(builder, "./res/ultramat_1_1.glade", &error) == 0)
    {
        g_print("Error loading ui-file!\n");
        g_clear_error(&error);

        return 1;
    }

    g_print("Hello from my GTK!\n");

    gtk_builder_connect_signals(builder, NULL);

    /* get Window from UI File and connect Exit-Button */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "Main_Window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    setupPage4(builder);

    /* apply css.style to window */
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(GTK_WIDGET(window));

    gtk_main();

    return 0;
}

void css_set(GtkCssProvider *cssProvider, GtkWidget *g_widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(g_widget);

    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}