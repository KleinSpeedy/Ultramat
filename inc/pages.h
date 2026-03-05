#ifndef PAGES_H
#define PAGES_H

#include <gtk/gtk.h>

// create the different stack pages and populate them with content
void create_stack_pages(GtkStack *mainStack, GtkListStore *recStore,
                        GtkListStore *ingPumpStore, GtkListStore *ingLiqStore);

#endif // PAGES_H
