/**
 * @file src/ggn-icon.c The GgnIcon object
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
 * SECTION:GgnIcon
 * @short_description: The status icon object
 * @include: src/ggn-icon.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-icon-priv.h"

/*
 * forward function definitions.
 */
static void ggn_icon_init (GgnIcon *self);
static void ggn_icon_class_init (GgnIconClass *klass);
static void ggn_icon_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnIcon, ggn_icon, G_TYPE_OBJECT);

/*
 * define the signals used.
 */
enum {
  CLICKED,
  CHECK_SELECTED,
  PREFS_SELECTED,
  ABOUT_SELECTED,
  QUIT_SELECTED,
  LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };


static void open_gmail_in_browser(void)
{
  fprintf(stderr, "Opening browser...\n");
  system("xdg-open \"https://mail.google.com\"");
}

/**
 * ggn_icon_default_clicked_cb:
 * @icon: The status icon which emitted the signal.
 *
 * This is the default "clicked" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_icon_default_clicked_cb (GgnIcon *icon) {
  open_gmail_in_browser();
}

/**
 * ggn_icon_default_check_selected_cb:
 * @icon: The status icon which emitted the signal.
 *
 * This is the default "check_selected" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_icon_default_check_selected_cb (GgnIcon *icon) {
  /* do nothing. */
}

/**
 * ggn_icon_default_prefs_selected_cb:
 * @icon: The status icon which emitted the signal.
 *
 * This is the default "prefs_selected" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_icon_default_prefs_selected_cb (GgnIcon *icon) {
  /* do nothing. */
}

/**
 * ggn_icon_default_about_selected_cb:
 * @icon: The status icon which emitted the signal.
 *
 * This is the default "about_selected" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_icon_default_about_selected_cb (GgnIcon *icon) {
  /* do nothing. */
}

/**
 * ggn_icon_default_quit_selected_cb:
 * @icon: The status icon which emitted the signal.
 *
 * This is the default "quit_selected" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_icon_default_quit_selected_cb (GgnIcon *icon) {
  /* do nothing. */
}

/**
 * ggn_icon_left_clicked:
 * @status_icon: The #GtkStatusIcon that started the callback.
 * @data: Our local data pointer.
 *
 * This function is called when the status icon is left-clicked.
 **/
static void ggn_icon_left_clicked (GtkStatusIcon *status_icon,
                                   gpointer data) {
  /* emit the appropriate signal. */
  g_signal_emit (GGN_ICON (data), signals[CLICKED], 0);
}

/**
 * ggn_icon_right_clicked:
 * @status_icon: The #GtkStatusIcon that started the callback.
 * @data: Our local data pointer.
 *
 * This function is called when the status icon is right-clicked.
 **/
static void ggn_icon_right_clicked (GtkStatusIcon *status_icon,
                                    guint button,
                                    guint time,
                                    gpointer data) {
  /* gain a reference to our icon. */
  GgnIcon *icon = GGN_ICON (data);

  /* bring up the dropdown menu. */
  gtk_menu_popup (icon->priv->menu, NULL, NULL,
                  gtk_status_icon_position_menu,
                  status_icon, button, time);
}

/**
 * ggn_icon_check_selected:
 * @item: The #GtkMenuItem that started the callback.
 * @data: Our local data pointer.
 *
 * This function is called when the "check" menu item inside the
 * status icon's dropdown menu is selected by the user.
 **/
G_MODULE_EXPORT
void ggn_icon_check_selected (GtkMenuItem *item, gpointer data) {
  /* emit the appropriate signal. */
  g_signal_emit (GGN_ICON (data), signals[CHECK_SELECTED], 0);
}

/**
 * ggn_icon_prefs_selected:
 * @item: The #GtkMenuItem that started the callback.
 * @data: Our local data pointer.
 *
 * This function is called when the "prefs" menu item inside the
 * status icon's dropdown menu is selected by the user.
 **/
G_MODULE_EXPORT
void ggn_icon_prefs_selected (GtkMenuItem *item, gpointer data) {
  /* emit the appropriate signal. */
  g_signal_emit (GGN_ICON (data), signals[PREFS_SELECTED], 0);
}

/**
 * ggn_icon_about_selected:
 * @item: The #GtkMenuItem that started the callback.
 * @data: Our local data pointer.
 *
 * This function is called when the "about" menu item inside the
 * status icon's dropdown menu is selected by the user.
 **/
G_MODULE_EXPORT
void ggn_icon_about_selected (GtkMenuItem *item, gpointer data) {
  /* emit the appropriate signal. */
  g_signal_emit (GGN_ICON (data), signals[ABOUT_SELECTED], 0);
}

/**
 * ggn_icon_quit_selected:
 * @item: The #GtkMenuItem that started the callback.
 * @data: Our local data pointer.
 *
 * This function is called when the "quit" menu item inside the
 * status icon's dropdown menu is selected by the user.
 **/
G_MODULE_EXPORT
void ggn_icon_quit_selected (GtkMenuItem *item, gpointer data) {
  /* emit the appropriate signal. */
  g_signal_emit (GGN_ICON (data), signals[QUIT_SELECTED], 0);
}

/*
 * ggn_icon_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_icon_init (GgnIcon *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnIconPrivate, 1);

  /* create the status icon. */
  self->priv->icon = gtk_status_icon_new ();

  /* link the left clicked callback to the icon. */
  g_signal_connect (G_OBJECT (self->priv->icon),
                    "activate",
                    G_CALLBACK (ggn_icon_left_clicked),
                    self);

  /* link the right clicked callback to the icon. */
  g_signal_connect (G_OBJECT (self->priv->icon),
                    "popup-menu",
                    G_CALLBACK (ggn_icon_right_clicked),
                    self);

  /* allocate a gtkbuilder object. */
  self->priv->xml = gtk_builder_new ();

  /* load the dropdown menu xml file. */
  gtk_builder_add_from_file (self->priv->xml, GGN_XMLUI_MENU, NULL);

  /* retrieve the dropdown menu widget. */
  self->priv->menu = GTK_MENU (gtk_builder_get_object (self->priv->xml,
                                                       "GgnContextMenu"));

  /* connect the signals used by the drop-down menu items. */
  gtk_builder_connect_signals (self->priv->xml, self);
}

/*
 * ggn_icon_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_icon_class_init (GgnIconClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_icon_finalize;

  /* setup the default signal handlers. */
  klass->clicked = ggn_icon_default_clicked_cb;
  klass->check_selected = ggn_icon_default_check_selected_cb;
  klass->prefs_selected = ggn_icon_default_prefs_selected_cb;
  klass->about_selected = ggn_icon_default_about_selected_cb;
  klass->quit_selected = ggn_icon_default_quit_selected_cb;

  /**
   * GgnIcon::clicked:
   * @icon: The #GgnIcon object that issued the callback.
   *
   * Emitted when the user left-clicks the status icon.
   **/
  signals[CLICKED] =
    g_signal_new ("clicked",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnIconClass, clicked),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * GgnIcon::check_selected:
   * @icon: The #GgnIcon object that issued the callback.
   *
   * Emitted when the user selects "check" from the dropdown
   * menu of the status icon that we are using.
   **/
  signals[CHECK_SELECTED] =
    g_signal_new ("check_selected",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnIconClass, check_selected),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * GgnIcon::prefs_selected:
   * @icon: The #GgnIcon object that issued the callback.
   *
   * Emitted when the user selects "prefs" from the dropdown
   * menu of the status icon that we are using.
   **/
  signals[PREFS_SELECTED] =
    g_signal_new ("prefs_selected",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnIconClass, prefs_selected),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * GgnIcon::about_selected:
   * @icon: The #GgnIcon object that issued the callback.
   *
   * Emitted when the user selects "about" from the dropdown
   * menu of the status icon that we are using.
   **/
  signals[ABOUT_SELECTED] =
    g_signal_new ("about_selected",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnIconClass, about_selected),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * GgnIcon::quit_selected:
   * @icon: The #GgnIcon object that issued the callback.
   *
   * Emitted when the user selects "quit" from the dropdown
   * menu of the status icon that we are using.
   **/
  signals[QUIT_SELECTED] =
    g_signal_new ("quit_selected",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnIconClass, quit_selected),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

/*
 * ggn_icon_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_icon_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnIcon *self = GGN_ICON (obj);

  /* free the status icon. */
  g_object_unref (G_OBJECT (self->priv->icon));

  /* free the strings. */
  g_free (self->priv->title);
  g_free (self->priv->summary);

  /* free the dropdown menu and its gtkbuilder. */
  g_object_unref (G_OBJECT (self->priv->xml));
  gtk_widget_destroy (GTK_WIDGET (self->priv->menu));

  /* free the left-click extended menu. */
  gtk_widget_destroy (GTK_WIDGET (self->priv->exmenu));

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_icon_parent_class)->finalize (obj);
}

/**
 * ggn_icon_new:
 *
 * Creates a new #GgnIcon with default values, which are
 * used to interact in the most basic way with the user.
 *
 * Returns: the new #GgnIcon object. Free with ggn_icon_free().
 **/
GgnIcon *ggn_icon_new (void) {
  /* make a newly created gobject. */
  GgnIcon *icon = g_object_new (GGN_TYPE_ICON, NULL);

  /* return the new object. */
  return icon;
}

/**
 * ggn_icon_free:
 * @icon: The #GgnIcon to free.
 *
 * Frees the given #GgnIcon object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_icon_free (GgnIcon *icon) {
  /* while this object exists... */
  while (G_IS_OBJECT (icon)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (icon));
  }
}

/**
 * ggn_icon_show:
 * @icon: The #GgnIcon to show.
 *
 * Shows the given #GgnIcon object in the panel tray.
 *
 * Returns: void.
 **/
void ggn_icon_show (GgnIcon *icon) {
  /* set the new visibility. */
  gtk_status_icon_set_visible (icon->priv->icon, TRUE);
}

/**
 * ggn_icon_hide:
 * @icon: The #GgnIcon to hide.
 *
 * Hides the given #GgnIcon object in the panel tray.
 *
 * Returns: void.
 **/
void ggn_icon_hide (GgnIcon *icon) {
  /* set the new visibility. */
  gtk_status_icon_set_visible (icon->priv->icon, FALSE);
}

/**
 * ggn_icon_set_style:
 * @icon: The #GgnIcon to modify.
 * @style: The new style to use.
 *
 * Sets a new style for the given #GgnIcon object.
 *
 * Returns: void.
 **/
void ggn_icon_set_style (GgnIcon *icon, guint style) {
  /* declare a helping variable. */
  gchar *filename = GGN_PIXMAP_NORMAL;

  /* don't bother if we are already at this style. */
  if (icon->priv->style == style)
    return;

  /* set the style variable. */
  icon->priv->style = style;

  /* determine the style to use. */
  if (style == GGN_ICON_STYLE_NORMAL) {
    /* set the icon filename. */
    filename = GGN_PIXMAP_NORMAL;
    icon->priv->urgency = NOTIFY_URGENCY_NORMAL;
  }
  else if (style == GGN_ICON_STYLE_ERROR) {
    /* set the icon filename. */
    filename = GGN_PIXMAP_ERROR;
    icon->priv->urgency = NOTIFY_URGENCY_CRITICAL;
  }
  else if (style == GGN_ICON_STYLE_WARNING) {
    /* set the icon filename. */
    filename = GGN_PIXMAP_WARNING;
    icon->priv->urgency = NOTIFY_URGENCY_NORMAL;
  }
  else if (style == GGN_ICON_STYLE_MESSAGE) {
    /* set the icon filename. */
    filename = GGN_PIXMAP_MESSAGE;
    icon->priv->urgency = NOTIFY_URGENCY_LOW;
  }
  else if (style == GGN_ICON_STYLE_CHECKING) {
    /* set the icon filename. */
    filename = GGN_PIXMAP_CHECKING;
    icon->priv->urgency = NOTIFY_URGENCY_NORMAL;
  }

  /* update the icon filename. */
  g_free (icon->priv->icon_file);
  icon->priv->icon_file = g_strdup (filename);

  /* update the gtk status icon. */
  gtk_status_icon_set_from_file (icon->priv->icon, icon->priv->icon_file);
}

/**
 * ggn_icon_set_tooltip:
 * @icon: The #GgnIcon to modify.
 * @tip: The new tooltip text.
 *
 * Sets a new tooltip for the given #GgnIcon object.
 *
 * Returns: void.
 **/
void ggn_icon_set_tooltip (GgnIcon *icon, const gchar *tip) {
  /* update the gtk status icon tooltip. */
  gtk_status_icon_set_tooltip (icon->priv->icon, tip);
}

/**
 * ggn_icon_set_title:
 * @icon: The #GgnIcon to modify.
 * @title: The new title text.
 *
 * Sets a new title for the notifications that arise out
 * of a given #GgnIcon object.
 *
 * Returns: void.
 **/
void ggn_icon_set_title (GgnIcon *icon, const gchar *title) {
  /* set the new title string. */
  g_free (icon->priv->title);
  icon->priv->title = g_strdup (title);
}

/**
 * ggn_icon_set_summary:
 * @icon: The #GgnIcon to modify.
 * @summary: The new summary text.
 *
 * Sets a new summary for the notifications that arise out
 * of a given #GgnIcon object.
 *
 * Returns: void.
 **/
void ggn_icon_set_summary (GgnIcon *icon, const gchar *summary) {
  /* set the new title string. */
  g_free (icon->priv->summary);
  icon->priv->summary = g_strdup (summary);
}

static void notif_click_to_open_gmail_cb(NotifyNotification *notification, char *action, gpointer user_data)
{
  fprintf(stderr, "Notification action: %s", action);
  open_gmail_in_browser();
}

/**
 * ggn_icon_show_notification_thread:
 * @data: The #gpointer to our thread data.
 *
 * This is the thread function that is called by the master
 * function, ggn_icon_show_notification.
 *
 * Returns: a pointer to the thread data.
 **/
gpointer ggn_icon_show_notification_thread (gpointer data) {
  /* lock the gtk thread. */
  gdk_threads_enter ();

  /* gain a reference to our icon. */
  GgnIcon *icon = GGN_ICON (data);

  /* wait until the icon is embedded. */
  while (!gtk_status_icon_is_embedded (icon->priv->icon)) {
    /* allow other events to occur. */
    if (gtk_events_pending ()) {
      /* iterate gtk. */
      gtk_main_iteration ();
    }
  }

  /* create a new notification. */
  icon->priv->note = notify_notification_new (icon->priv->title,
                                              icon->priv->summary,
                                              icon->priv->icon_file);

  /* attach the notification to our icon. */
//  notify_notification_attach_to_status_icon (icon->priv->note,
//                                             icon->priv->icon);

  /* status icon was removed */

  notify_notification_add_action (icon->priv->note,
                                  "open_gmail",
                                  "Open",
                                  notif_click_to_open_gmail_cb,
                                  NULL,
                                  NULL);


  /* set the notification timeout. */
  notify_notification_set_timeout (icon->priv->note, (4 * 1000));

  /* set the notification urgency. */
  notify_notification_set_urgency (icon->priv->note, icon->priv->urgency);

  /* show the notification. */
  notify_notification_show (icon->priv->note, NULL);

  /* unlock the gtk thread. */
  gdk_threads_leave ();

  /* return from our thread. */
  return data;
}

/**
 * ggn_icon_show_notification:
 * @icon: The #GgnIcon to utilize.
 *
 * Shows a bubble notification from the given #GgnIcon using
 * the currently set style, title, and summary.
 *
 * Returns: void.
 **/
void ggn_icon_show_notification (GgnIcon *icon) {
  /* ensure that libnotify is initialized. */
  if (!notify_is_initted ())
    notify_init (g_get_prgname ());

  /* execute the notification bubble thread. */
  g_thread_create (ggn_icon_show_notification_thread,
                   (gpointer) icon, FALSE, NULL);
}

