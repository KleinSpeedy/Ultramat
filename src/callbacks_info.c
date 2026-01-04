#include "callbacks_info.h"

#include <assert.h>

static GtkLabel *mcVersionLabel_ = NULL;

void cb_info_set_mc_version_label(GtkLabel *label)
{
    assert(label != NULL);
    mcVersionLabel_ = label;
    g_object_add_weak_pointer(G_OBJECT(label), (gpointer *)&mcVersionLabel_);
}

void cb_info_set_mc_version(const struct SemanticVersion vers)
{
    char versionBuffer[14];
    memset(versionBuffer, 0, sizeof(versionBuffer));
    snprintf(versionBuffer, sizeof(versionBuffer), "v%d.%d.%d", vers.major,
             vers.minor, vers.bugfix);

    gtk_label_set_text(mcVersionLabel_, versionBuffer);
}

void cb_info_app_update_button_clicked(GtkButton *button)
{
    (void)button;
    g_print("App update requested\n");
}

void cb_info_mc_update_button_clicked(GtkButton *button)
{
    (void)button;
    g_print("MC update requested\n");
}
