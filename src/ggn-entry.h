/**
 * @file src/ggn-entry.h The GgnEntry object
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

/*
 * ensure this is a once-only header.
 */
#ifndef __GNOME_GMAIL_NOTIFIER_ENTRY_H__
#define __GNOME_GMAIL_NOTIFIER_ENTRY_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * the entry state vales.
 */
#define GGN_ENTRY_STATE_NEW     0
#define GGN_ENTRY_STATE_UNREAD  1
#define GGN_ENTRY_STATE_READ    2

/*
 * object function macros.
 */
#define GGN_TYPE_ENTRY (ggn_entry_get_type ())
#define GGN_ENTRY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_ENTRY, GgnEntry))
#define GGN_ENTRY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_ENTRY, GgnEntryClass))
#define GGN_IS_ENTRY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_ENTRY))
#define GGN_IS_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_ENTRY))
#define GGN_ENTRY_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_ENTRY, GgnEntryClass))

/*
 * type macros.
 */
typedef struct _GgnEntry GgnEntry;
typedef struct _GgnEntryClass GgnEntryClass;
typedef struct _GgnEntryPrivate GgnEntryPrivate;

/*
 * object definition.
 */
struct _GgnEntry {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnEntryPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnEntryClass {
  /* parent gobject class. */
  GObjectClass parent;
};

/*
 * gobject-type definition.
 */
GType ggn_entry_get_type (void);

/*
 * object creation functions.
 */
GgnEntry *ggn_entry_new (void);
void ggn_entry_free (GgnEntry *entry);

/*
 * property retrieval functions.
 */
gchar *ggn_entry_get_summary (GgnEntry *entry);
gchar *ggn_entry_get_title (GgnEntry *entry);
gchar *ggn_entry_get_sender_name (GgnEntry *entry);
gchar *ggn_entry_get_sender_email (GgnEntry *entry);
gchar *ggn_entry_get_link (GgnEntry *entry);
gchar *ggn_entry_get_id (GgnEntry *entry);

/*
 * state retrieval function.
 */
guint ggn_entry_get_state (GgnEntry *entry);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_ENTRY_H__ */

