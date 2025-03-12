#include "drinks.h"
#include "drinklists.h"
#include "util/dynamic_array.h"
#include "gui.h"

#include <gtk/gtk.h>

int main(int argc, char **argv)
{
    // start by initializing GUI-GTK thread
    if(!gtk_init_check(&argc, &argv))
        return EXIT_FAILURE;

    // Read ingredients and recipes from files
    VLArray_t *ingArray = drinks_io_read_ingredients();
    VLArray_t *recArray = drinks_io_read_recipes();
    if(ingArray == NULL || recArray == NULL)
    {
        // TODO: Error log
        return EXIT_FAILURE;
    }

    lists_create_ingredient_store(ingArray);
    lists_create_recipe_store(recArray);

    gui_thread();

    vla_free(ingArray);
    // TODO: Recipes have dynamic allocations themselves, provide custom free callback
    vla_free(recArray);

    return 0;
}
