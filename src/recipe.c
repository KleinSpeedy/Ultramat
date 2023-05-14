//
// Source file for recipe object
//

// Module include
#include "recipe.h"

struct _URecipe
{
    GObject *parent;

    gchar *name;        /* Recipe name */
    guint id;           /* Recipe identifier */
    gboolean available; /* Is recipe currently available or not*/
};

G_DEFINE_TYPE(URecipe, u_recipe, G_TYPE_OBJECT)

static void
u_recipe_class_init(URecipeClass *class)
{
}

static void
u_recipe_init(URecipe *rec)
{
}

gchar *
u_recipe_get_name(URecipe *self)
{
    g_assert_nonnull(self);
    if(!self)
        return "";
    return self->name;
}
void
u_recipe_set_name(URecipe *self, gchar *name)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->name = name;
}

guint
u_recipe_get_id(URecipe *self)
{
    g_assert_nonnull(self);
    if(!self)
        return 0;
    return self->id;
}
void
u_recipe_set_id(URecipe *self, uint id)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->id = id;
}

gboolean
u_recipe_is_available(URecipe *self)
{
    g_assert_nonnull(self);
    if(!self)
        return FALSE;
    return self->available;
}

void
u_recipe_set_available(URecipe *self, gboolean toSet)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->available = toSet;
}

URecipe *
u_recipe_new(gchar *name, guint id, gboolean available)
{
    URecipe *rec;
    rec = g_object_new(U_TYPE_RECIPE, NULL);
    g_assert_nonnull(rec);

    rec->name = name;
    rec->id = id;
    rec->available = available;
    return rec;
}