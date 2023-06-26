#ifndef __PAGES_H__
#define __PAGES_H__

#include <gtk/gtk.h>

// ComboBox positions on page one, used for callbacks and
// keeping track of ingredients positions
typedef enum
{
    PAGES_COMBO_POS_INVALID = -1,
    PAGES_COMBO_POS_ONE,
    PAGES_COMBO_POS_TWO,
    PAGES_COMBO_POS_THREE,
    PAGES_COMBO_POS_FOUR,
    PAGES_COMBO_POS_FIVE,
    PAGES_COMBO_POS_SIX,
    //PAGES_COMBO_POS_SEVEN, //TODO: Add this when adding pressure pumps

    PAGES_COMBO_POS_NUM
} ComboPositions;

// create the different stack pages and populate them with content
void createStackPages(GtkStack *mainStack);

#endif //__PAGES_H__
