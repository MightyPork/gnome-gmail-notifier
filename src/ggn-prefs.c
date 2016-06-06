/**
 * @file src/ggn-prefs.c The GgnPrefs object
 * @Copyright (C) 2010 Bradley A. Worley.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * 
 *     Free Software Foundation, Inc.
 *     59 Temple Place, Suite 330
 *     Boston, MA 02111-1307 USA
 **/

/**
 * SECTION:GgnPrefs
 * @short_description: The configuration access object
 * @include: src/ggn-prefs.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-prefs-priv.h"

/*
 * forward function definitions.
 */
static void ggn_prefs_init (GgnPrefs *self);
static void ggn_prefs_class_init (GgnPrefsClass *klass);
static void ggn_prefs_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnPrefs, ggn_prefs, G_TYPE_OBJECT);

/*
 * ggn_prefs_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_prefs_init (GgnPrefs *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnPrefsPrivate, 1);

  /* setup the gconf2 client object. */
  self->priv->conf = gconf_client_get_default ();
}

/*
 * ggn_prefs_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_prefs_class_init (GgnPrefsClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_prefs_finalize;
}

/*
 * ggn_prefs_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_prefs_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnPrefs *self = GGN_PREFS (obj);

  /* free the private instance objects. */
  g_object_unref (G_OBJECT (self->priv->conf));

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_prefs_parent_class)->finalize (obj);
}

/**
 * ggn_prefs_new:
 *
 * Creates a new #GgnPrefs with default values, which are
 * used to access and modify configuration values for the
 * notifier application behavior.
 *
 * Returns: the new #GgnPrefs object. Free with ggn_prefs_free().
 **/
GgnPrefs *ggn_prefs_new (void) {
  /* make a newly created gobject. */
  GgnPrefs *prefs = g_object_new (GGN_TYPE_PREFS, NULL);

  /* return the new object. */
  return prefs;
}

/**
 * ggn_prefs_free:
 * @prefs: The #GgnPrefs to free.
 *
 * Frees the given #GgnPrefs object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_prefs_free (GgnPrefs *prefs) {
  /* while this object exists... */
  while (G_IS_OBJECT (prefs)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (prefs));
  }
}

/**
 * ggn_prefs_get_int:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 *
 * Utilizes the #GgnPrefs object to access a #gint that is
 * saved in the GConf2 database.
 *
 * Returns: the #gint value.
 **/
gint ggn_prefs_get_int (GgnPrefs *prefs, const gchar *key) {
  /* declare a helping variable. */
  gint value;

  /* query gconf for the value. */
  value = gconf_client_get_int (prefs->priv->conf, key, NULL);

  /* check for bounds. */
  if (g_utf8_collate (key, GGN_PREF_CHECKRATE) == 0 && value <= 0) {
    /* ensure the check rate is a default of 10. */
    value = 10;
  }

  /* return the value. */
  return value;
}

/**
 * ggn_prefs_set_int:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 * @value: The new value of @key to set.
 *
 * Utilizes the #GgnPrefs object to set a #gint to be saved
 * in the GConf2 database.
 *
 * Returns: void.
 **/
void ggn_prefs_set_int (GgnPrefs *prefs, const gchar *key, gint value) {
  /* set the new value in gconf. */
  gconf_client_set_int (prefs->priv->conf, key, value, NULL);
}

/**
 * ggn_prefs_get_bool:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 *
 * Utilizes the #GgnPrefs object to access a #gboolean that is
 * saved in the GConf2 database.
 *
 * Returns: the #gboolean value.
 **/
gboolean ggn_prefs_get_bool (GgnPrefs *prefs, const gchar *key) {
  /* declare a helping variable. */
  gboolean value;

  /* query gconf for the value. */
  value = gconf_client_get_bool (prefs->priv->conf, key, NULL);

  /* return the value. */
  return value;
}

/**
 * ggn_prefs_set_bool:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 * @value: The new value of @key to set.
 *
 * Utilizes the #GgnPrefs object to set a #gboolean to be saved
 * in the GConf2 database.
 *
 * Returns: void.
 **/
void ggn_prefs_set_bool (GgnPrefs *prefs, const gchar *key, gboolean value) {
  /* set the new value in gconf. */
  gconf_client_set_bool (prefs->priv->conf, key, value, NULL);
}

/**
 * ggn_prefs_get_list:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 *
 * Utilizes the #GgnPrefs object to access a #GSList that is
 * saved in the GConf2 database.
 *
 * Returns: the #GSList value.
 **/
GSList *ggn_prefs_get_list (GgnPrefs *prefs, const gchar *key) {
  /* declare a helping variable. */
  GSList *value;

  /* query gconf for the value. */
  value = gconf_client_get_list (prefs->priv->conf, key,
                                 GCONF_VALUE_STRING, NULL);

  /* return the value. */
  return value;
}

/**
 * ggn_prefs_set_list:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 * @value: The new value of @key to set.
 *
 * Utilizes the #GgnPrefs object to set a #GSList to be saved
 * in the GConf2 database.
 *
 * Returns: void.
 **/
void ggn_prefs_set_list (GgnPrefs *prefs, const gchar *key, GSList *value) {
  /* set the new value in gconf. */
  gconf_client_set_list (prefs->priv->conf, key,
                         GCONF_VALUE_STRING,
                         value, NULL);
}

/**
 * ggn_prefs_get_string:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 *
 * Utilizes the #GgnPrefs object to access a #gchar* that is
 * saved in the GConf2 database.
 *
 * Returns: the #gchar* value. Free with g_free().
 **/
gchar *ggn_prefs_get_string (GgnPrefs *prefs, const gchar *key) {
  /* declare a helping variable. */
  gchar *value;

  /* query gconf for the value. */
  value = gconf_client_get_string (prefs->priv->conf, key, NULL);

  /* return the value. */
  return value;
}

/**
 * ggn_prefs_set_string:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 * @value: The new value of @key to set.
 *
 * Utilizes the #GgnPrefs object to set a #gchar* to be saved
 * in the GConf2 database.
 *
 * Returns: void.
 **/
void ggn_prefs_set_string (GgnPrefs *prefs,
                           const gchar *key,
                           const gchar *value) {
  /* ensure the string is not null. */
  if (!value)
    return;

  /* set the new value in gconf. */
  gconf_client_set_string (prefs->priv->conf, key, value, NULL);
}

/**
 * ggn_prefs_unset:
 * @prefs: The #GgnPrefs to utilize.
 * @key: The GConf2 key string to access.
 *
 * Utilizes the #GgnPrefs object to recursively unset (delete) a tree
 * from the GConf2 database.
 *
 * Returns: void.
 **/
void ggn_prefs_unset (GgnPrefs *prefs, const gchar *key) {
  /* defer to the higher power. */
  gconf_client_recursive_unset (prefs->priv->conf, key, 0, NULL);
}

