/**
 * @file src/ggn-icon-priv.h The GgnIcon object
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
 * GgnIconPrivate:
 * @icon: Our #GtkStatusIcon that we use.
 * @note: Our #NotifyNotification that we use.
 * @style: The style of our @icon and @note objects.
 * @title: The title for the notifications.
 * @summary: The summary for the notifications.
 * @icon_file: The status icon image filename.
 * @urgency: The urgency for the notifications.
 * @xml: The #GtkBuilder xml object for the menu.
 * @menu: The #GtkMenu that the notifier uses.
 * @exmenu: The extended #GtkMenu that the notifier uses.
 *
 * The #GgnIcon is a GObject that is concerned with the status icon
 * for the GNOME GMail Notifier and its associated notification
 * bubbles.
 **/
struct _GgnIconPrivate {
  /* the tray icon and notification. */
  GtkStatusIcon *icon;
  NotifyNotification *note;

  /* the stylistic variables. */
  guint style;
  gchar *title;
  gchar *summary;
  gchar *icon_file;
  NotifyUrgency urgency;

  /* the gtkbuilder xml object and menu. */
  GtkBuilder *xml;
  GtkMenu *menu;

  /* the extended menu. */
  GtkMenu *exmenu;
};

