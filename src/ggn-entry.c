/**
 * @file src/ggn-entry.c The GgnEntry object
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
 * SECTION:GgnEntry
 * @short_description: The account entry object
 * @include: src/ggn-entry.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-entry-priv.h"

/*
 * forward function definitions.
 */
static void ggn_entry_init (GgnEntry *self);
static void ggn_entry_class_init (GgnEntryClass *klass);
static void ggn_entry_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnEntry, ggn_entry, G_TYPE_OBJECT);

/*
 * ggn_entry_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_entry_init (GgnEntry *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnEntryPrivate, 1);

  /* initialize private instance objects. */
  self->priv->summary = NULL;
  self->priv->title = NULL;
  self->priv->sender_email = NULL;
  self->priv->sender_name = NULL;
  self->priv->link = NULL;
  self->priv->id = NULL;

  /* initialize the state. */
  self->priv->state = GGN_ENTRY_STATE_NEW;
}

/*
 * ggn_entry_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_entry_class_init (GgnEntryClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_entry_finalize;
}

/*
 * ggn_entry_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_entry_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnEntry *self = GGN_ENTRY (obj);

  /* free the private instance objects. */
  g_free (self->priv->summary);
  g_free (self->priv->title);
  g_free (self->priv->sender_email);
  g_free (self->priv->sender_name);
  g_free (self->priv->link);
  g_free (self->priv->id);

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_entry_parent_class)->finalize (obj);
}

/**
 * ggn_entry_new:
 *
 * Creates a new #GgnEntry with default values, which are
 * used to show information about the current inbox new messages.
 *
 * Returns: the new #GgnEntry object. Free with ggn_entry_free().
 **/
GgnEntry *ggn_entry_new (void) {
  /* make a newly created gobject. */
  GgnEntry *entry = g_object_new (GGN_TYPE_ENTRY, NULL);

  /* return the new object. */
  return entry;
}

/**
 * ggn_entry_free:
 * @entry: The #GgnEntry to free.
 *
 * Frees the given #GgnEntry object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_entry_free (GgnEntry *entry) {
  /* while this object exists... */
  while (G_IS_OBJECT (entry)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (entry));
  }
}

/**
 * ggn_entry_get_summary:
 * @entry: The #GgnEntry to access.
 *
 * Request the summary string of the #GgnEntry object.
 *
 * Returns: An in-use string. Freed automatically by ggn_entry_free().
 **/
gchar *ggn_entry_get_summary (GgnEntry *entry) {
  /* return the value we are storing. */
  return entry->priv->summary;
}

/**
 * ggn_entry_get_title:
 * @entry: The #GgnEntry to access.
 *
 * Request the title string of the #GgnEntry object.
 *
 * Returns: An in-use string. Freed automatically by ggn_entry_free().
 **/
gchar *ggn_entry_get_title (GgnEntry *entry) {
  /* return the value we are storing. */
  return entry->priv->title;
}

/**
 * ggn_entry_get_sender_email:
 * @entry: The #GgnEntry to access.
 *
 * Request the sender email string of the #GgnEntry object.
 *
 * Returns: An in-use string. Freed automatically by ggn_entry_free().
 **/
gchar *ggn_entry_get_sender_email (GgnEntry *entry) {
  /* return the value we are storing. */
  return entry->priv->sender_email;
}

/**
 * ggn_entry_get_sender_name:
 * @entry: The #GgnEntry to access.
 *
 * Request the sender name string of the #GgnEntry object.
 *
 * Returns: An in-use string. Freed automatically by ggn_entry_free().
 **/
gchar *ggn_entry_get_sender_name (GgnEntry *entry) {
  /* return the value we are storing. */
  return entry->priv->sender_name;
}

/**
 * ggn_entry_get_link:
 * @entry: The #GgnEntry to access.
 *
 * Request the link string of the #GgnEntry object.
 *
 * Returns: An in-use string. Freed automatically by ggn_entry_free().
 **/
gchar *ggn_entry_get_link (GgnEntry *entry) {
  /* return the value we are storing. */
  return entry->priv->link;
}

/**
 * ggn_entry_get_id:
 * @entry: The #GgnEntry to access.
 *
 * Request the id string of the #GgnEntry object.
 *
 * Returns: An in-use string. Freed automatically by ggn_entry_free().
 **/
gchar *ggn_entry_get_id (GgnEntry *entry) {
  /* return the value we are storing. */
  return entry->priv->id;
}

/**
 * ggn_entry_get_state:
 * @entry: The #GgnEntry to access.
 *
 * Request the state of the #GgnEntry object.
 *
 * Returns: The state #guint of the entry object.
 **/
guint ggn_entry_get_state (GgnEntry *entry) {
  /* return the value we are storing. */
  return entry->priv->state;
}

