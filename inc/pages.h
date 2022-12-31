#ifndef __PAGES_H__
#define __PAGES_H__

#include <gtk/gtk.h>
#include "helper.h"

// create the different stack pages and populate them with content
void createStackPages(GtkStack *mainStack, GtkListStore *ingListStore, GtkListStore *recListStore);

#endif //__PAGES_H__