#include <gtk/gtk.h>

#include "inc/Gui.h"
#include "inc/SerialHandler.h"
#include "inc/DrinkManagement.h"

int main(int argc, char **argv)
{
    setupRecipesAndDrinks();

    handleGtk(argc,argv);

    return EXIT_SUCCESS;
}

/*
void css_set(GtkCssProvider *prov, GtkWidget *widg);

void css_set(GtkCssProvider *cssProvider, GtkWidget *g_widget)
{
    GtkStyleContext *context = gtk_widget_get_style_context(g_widget);

    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
*/