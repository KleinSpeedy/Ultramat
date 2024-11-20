#include "drinks.h"
#include "drinklists.h"
#include "dynamic_array.h"
#include "gui.h"

#include <gtk/gtk.h>

int main(int argc, char **argv)
{
    // start by initializing GUI-GTK thread
    if(!gtk_init_check(&argc, &argv))
        return EXIT_FAILURE;

    // TODO: Delete arrays and their possible dynamic allocs before exiting

    // Read ingredients and recipes from files
    VLArray_t *ingArray = drinks_io_read_ingredients();
    if(ingArray == NULL)
    {
        // TODO: Error log
        return EXIT_FAILURE;
    }
    
    VLArray_t *recArray = drinks_io_read_recipes();
    if(recArray == NULL)
    {
        // TODO: Error log
        return EXIT_FAILURE;
    }

    lists_create_ingredient_store(ingArray);
    lists_create_recipe_store(recArray);

    gui_thread();

    return 0;
}
