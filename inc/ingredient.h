#ifndef __INGREDIENT_H__
#define __INGREDIENT_H__

/* Object representing an ingredient */

#include <glib-object.h>

G_BEGIN_DECLS

/* Create Ultramat (U) namespace ingredient objects */

#define U_TYPE_INGREDIENT (u_ingredient_get_type ())
G_DECLARE_FINAL_TYPE (UIngredient, u_ingredient, U, INGREDIENT, GObject)

/* Getter/Setter for UIngredient members */

guint
u_ingredient_get_id(UIngredient *self);
void
u_ingredient_set_id(UIngredient *self, guint id);

guint8
u_ingredient_get_position(UIngredient *self);
void
u_ingredient_set_position(UIngredient *self, guint8 pos);

gboolean
u_ingredient_get_selected(UIngredient *self);
void
u_ingredient_set_selected(UIngredient *self, gboolean toSet);

const
gchar *u_ingredient_get_name(UIngredient *self);
void
u_ingredient_set_name(UIngredient *self, gchar *name);

UIngredient  *
u_ingredient_new(gchar *name, guint id, guint8 position, gboolean selected);

G_END_DECLS

#endif //__INGREDIENT_H__
