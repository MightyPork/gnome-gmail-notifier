/**
 * @file src/ggn-icon.h The GgnIcon object
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
#ifndef __GNOME_GMAIL_NOTIFIER_ICON_H__
#define __GNOME_GMAIL_NOTIFIER_ICON_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * notify style definitions.
 */
enum {
  GGN_ICON_STYLE_NONE,
  GGN_ICON_STYLE_NORMAL,
  GGN_ICON_STYLE_ERROR,
  GGN_ICON_STYLE_WARNING,
  GGN_ICON_STYLE_MESSAGE,
  GGN_ICON_STYLE_CHECKING
};

/*
 * object function macros.
 */
#define GGN_TYPE_ICON (ggn_icon_get_type ())
#define GGN_ICON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_ICON, GgnIcon))
#define GGN_ICON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_ICON, GgnIconClass))
#define GGN_IS_ICON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_ICON))
#define GGN_IS_ICON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_ICON))
#define GGN_ICON_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_ICON, GgnIconClass))

/*
 * type macros.
 */
typedef struct _GgnIcon GgnIcon;
typedef struct _GgnIconClass GgnIconClass;
typedef struct _GgnIconPrivate GgnIconPrivate;

/*
 * object definition.
 */
struct _GgnIcon {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnIconPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnIconClass {
  /* parent gobject class. */
  GObjectClass parent;

  /* status icon and menu callbacks. */
  void (*clicked) (GgnIcon *icon);
  void (*check_selected) (GgnIcon *icon);
  void (*prefs_selected) (GgnIcon *icon);
  void (*about_selected) (GgnIcon *icon);
  void (*quit_selected) (GgnIcon *icon);
};

/*
 * gobject-type definition.
 */
GType ggn_icon_get_type (void);

/*
 * object creation functions.
 */
GgnIcon *ggn_icon_new (void);
void ggn_icon_free (GgnIcon *icon);

/*
 * icon visibility functions.
 */
void ggn_icon_show (GgnIcon *icon);
void ggn_icon_hide (GgnIcon *icon);

/*
 * style variable setting functions.
 */
void ggn_icon_set_style (GgnIcon *icon, guint style);
void ggn_icon_set_tooltip (GgnIcon *icon, const gchar *tip);
void ggn_icon_set_title (GgnIcon *icon, const gchar *title);
void ggn_icon_set_summary (GgnIcon *icon, const gchar *summary);

/*
 * notification function.
 */
void ggn_icon_show_notification (GgnIcon *icon);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_ICON_H__ */

