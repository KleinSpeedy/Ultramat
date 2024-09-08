/**
 * Recipe objects represent one drink and the needed ingredients
 */

#ifndef __RECIPE_H__
#define __RECIPE_H__

#include <glib-object.h>
#include "ingredient.h"

G_BEGIN_DECLS

/* Create Ultramat (U) namespace recipe objects */

#define U_TYPE_RECIPE (u_recipe_get_type ())
G_DECLARE_FINAL_TYPE(URecipe, u_recipe, U, RECIPE, GObject)

const gchar *
u_recipe_get_name(URecipe *self);
void
u_recipe_set_name(URecipe *self, const gchar *name);

guint
u_recipe_get_id(URecipe *self);
void
u_recipe_set_id(URecipe *self, uint id);

gboolean
u_recipe_is_available(URecipe *self);
void
u_recipe_set_available(URecipe *self, gboolean toSet);

guint
u_recipe_get_ingredient_count(URecipe *self);

void
u_recipe_set_selected(URecipe *rec, gboolean toSet);
gboolean
u_recipe_is_selected(URecipe *self);

URecipe *
u_recipe_new(const gchar *name, guint id, gboolean available);

/* Manage list of Ingredients of each recipe as GLib LinkedList */
gboolean
u_recipe_append_ingredient(URecipe *rec, UIngredient *ing, guint8 quantity);

gboolean
u_recipe_get_nth_ingredient_info(URecipe *rec, guint idx, gint8 *pos, guint *quantity);

G_END_DECLS

#ifdef ULTRA_DEBUG
void
dbg_print_recipe(URecipe *rec);
#endif // ULTRA_DEBUG
#endif //__RECIPE_H__
