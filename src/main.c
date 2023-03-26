/* Main File */
#include "gui.h"
#include "serialcom.h"

int main(int argc, char **argv)
{
    // start threads
    serialcom_start_handler_thread();
    guiHandler(argc, argv);

    // stop serial thread before exiting
    serialcom_stop_handler_thread();
    return 0;
}
