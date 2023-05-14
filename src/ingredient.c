/**
 * Source file for ingredient object
 */

/* Module include */
#include "ingredient.h"

struct _UIngredient
{
    GObject parent;

    gchar *name;        /* Name of ingredient */
    guint id;           /* ingredient identifier */
    guint8 position;    /* position on axis */
    gboolean selected;  /* ingredient currently selected or not */
};

G_DEFINE_TYPE (UIngredient, u_ingredient, G_TYPE_OBJECT)

static void
u_ingredient_class_init(UIngredientClass *class)
{
}

static void
u_ingredient_init(UIngredient *ing)
{
}

guint
u_ingredient_get_id(UIngredient *self)
{
    g_assert_nonnull(self);
    if(!self)
        return 0;
    return self->id;
}

void
u_ingredient_set_id(UIngredient *self, guint id)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->id = id;
}

guint8
u_ingredient_get_position(UIngredient *self)
{
    g_assert_nonnull(self);
    if(!self)
        return 0;
    return self->position;
}
void
u_ingredient_set_position(UIngredient *self, guint8 pos)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->position = pos;
}

gboolean
u_ingredient_get_selected(UIngredient *self)
{
    g_assert_nonnull(self);
    if(!self)
        return FALSE;
    return self->selected;
}
void
u_ingredient_set_selected(UIngredient *self, gboolean toSet)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->selected = toSet;
}

const
gchar *u_ingredient_get_name(UIngredient *self)
{
    g_assert_nonnull(self);
    if(!self)
        return "";
    return self->name;
}
void
u_ingredient_set_name(UIngredient *self, gchar *name)
{
    g_assert_nonnull(self);
    if(!self)
        return;
    self->name = name;
}

UIngredient  *
u_ingredient_new(gchar *name, guint id, guint8 position, gboolean selected)
{
    UIngredient *ing;
    ing = g_object_new(U_TYPE_INGREDIENT, NULL);
    g_assert_nonnull(ing);

    ing->name = name;
    ing->id = id;
    ing->position = position;
    ing->selected = selected;
    return ing;
}