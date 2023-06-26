/* Main File */
#include "drinklists.h"
#include "drinks.h"
#include "serialcom.h"
#include "gui.h"

int main(int argc, char **argv)
{
    // Create ListStores holding drink information
    lists_create_ingredient_store();
    lists_create_recipe_store();
    // Read ingredients and recipes from files
    if(drinks_io_read_ingredients() != DRINKS_OK)
        return EXIT_FAILURE;
    if(drinks_io_read_recipes() != DRINKS_OK)
        return EXIT_FAILURE;

    // start threads
    serialcom_start_handler_thread();
    guiHandler(argc, argv);

    // stop serial thread before exiting
    serialcom_stop_handler_thread();
    return 0;
}
