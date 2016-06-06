/**
 * @file src/ggn-prefs-window.h The GgnPrefsWindow object
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
#ifndef __GNOME_GMAIL_NOTIFIER_PREFS_WINDOW_H__
#define __GNOME_GMAIL_NOTIFIER_PREFS_WINDOW_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * object function macros.
 */
#define GGN_TYPE_PREFS_WINDOW (ggn_prefs_window_get_type ())
#define GGN_PREFS_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_PREFS_WINDOW, GgnPrefsWindow))
#define GGN_PREFS_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_PREFS_WINDOW, GgnPrefsWindowClass))
#define GGN_IS_PREFS_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_PREFS_WINDOW))
#define GGN_IS_PREFS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_PREFS_WINDOW))
#define GGN_PREFS_WINDOW_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_PREFS_WINDOW, GgnPrefsWindowClass))

/*
 * type macros.
 */
typedef struct _GgnPrefsWindow GgnPrefsWindow;
typedef struct _GgnPrefsWindowClass GgnPrefsWindowClass;
typedef struct _GgnPrefsWindowPrivate GgnPrefsWindowPrivate;

/*
 * object definition.
 */
struct _GgnPrefsWindow {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnPrefsWindowPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnPrefsWindowClass {
  /* parent gobject class. */
  GObjectClass parent;
};

/*
 * gobject-type definition.
 */
GType ggn_prefs_window_get_type (void);

/*
 * object creation functions.
 */
GgnPrefsWindow *ggn_prefs_window_new (void);
GgnPrefsWindow *ggn_prefs_window_new_from_manager (GgnManager *manager);
void ggn_prefs_window_free (GgnPrefsWindow *window);

/*
 * window show/hide functions.
 */
void ggn_prefs_window_show (GgnPrefsWindow *window);
void ggn_prefs_window_hide (GgnPrefsWindow *window);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_PREFS_WINDOW_H__ */

