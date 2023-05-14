/**
 * Recipe objects represent one drink and the needed ingredients
 */

#ifndef __RECIPE_H__
#define __RECIPE_H__

#include <glib-object.h>

G_BEGIN_DECLS

/* Create Ultramat (U) namespace recipe objects */

#define U_TYPE_RECIPE (u_recipe_get_type ())
G_DECLARE_FINAL_TYPE(URecipe, u_recipe, U, RECIPE, GObject)

gchar *
u_recipe_get_name(URecipe *self);
void
u_recipe_set_name(URecipe *self, gchar *name);

guint
u_recipe_get_id(URecipe *self);
void
u_recipe_set_id(URecipe *self, uint id);

gboolean
u_recipe_is_available(URecipe *self);
void
u_recipe_set_available(URecipe *self, gboolean toSet);

URecipe *
u_recipe_new(gchar *name, guint id, gboolean available);

G_END_DECLS

#endif //__RECIPE_H__
