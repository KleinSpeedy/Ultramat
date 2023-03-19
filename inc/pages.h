#ifndef __PAGES_H__
#define __PAGES_H__

#include <gtk/gtk.h>
#include "drinks.h"
#include "drinklists.h"

// create the different stack pages and populate them with content
void createStackPages(GtkStack *mainStack, struct DrinkManagement *dm);

#endif //__PAGES_H__
