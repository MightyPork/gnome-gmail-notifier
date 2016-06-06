/**
 * @file src/ggn-manager.h The GgnManager object
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
#ifndef __GNOME_GMAIL_NOTIFIER_MANAGER_H__
#define __GNOME_GMAIL_NOTIFIER_MANAGER_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * object function macros.
 */
#define GGN_TYPE_MANAGER (ggn_manager_get_type ())
#define GGN_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_MANAGER, GgnManager))
#define GGN_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_MANAGER, GgnManagerClass))
#define GGN_IS_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_MANAGER))
#define GGN_IS_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_MANAGER))
#define GGN_MANAGER_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_MANAGER, GgnManagerClass))

/*
 * type macros.
 */
typedef struct _GgnManager GgnManager;
typedef struct _GgnManagerClass GgnManagerClass;
typedef struct _GgnManagerPrivate GgnManagerPrivate;

/*
 * object definition.
 */
struct _GgnManager {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnManagerPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnManagerClass {
  /* parent gobject class. */
  GObjectClass parent;
};

/*
 * gobject-type definition.
 */
GType ggn_manager_get_type (void);

/*
 * object creation functions.
 */
GgnManager *ggn_manager_new (void);
void ggn_manager_free (GgnManager *manager);

/*
 * main application control functions.
 */
void ggn_manager_main (GgnManager *manager, int argc, char **argv);
void ggn_manager_quit (GgnManager *manager);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_MANAGER_H__ */

