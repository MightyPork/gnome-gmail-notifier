/**
 * @file src/ggn-manager.c The GgnManager object
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
 * SECTION:GgnManager
 * @short_description: The coordinator object
 * @include: src/ggn-manager.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-manager-priv.h"
#include "ggn-icon-priv.h"

/*
 * forward function definitions.
 */
static void ggn_manager_init (GgnManager *self);
static void ggn_manager_class_init (GgnManagerClass *klass);
static void ggn_manager_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnManager, ggn_manager, G_TYPE_OBJECT);

/*
 * define the constant-valued strings to use for notification.
 */
#define GGN_MANAGER_TOOLTIP_CHECKING _("Checking mail...")
#define GGN_MANAGER_TOOLTIP_NOACCTS _("No accounts configured")
#define GGN_MANAGER_TOOLTIP_FAILURE _("Unable to check mail")
#define GGN_MANAGER_TOOLTIP_EMPTY _("No new messages")
#define GGN_MANAGER_TITLE_NOACCTS _("No accounts are configured")
#define GGN_MANAGER_TITLE_FAILURE _("Unable to check mail")
#define GGN_MANAGER_TITLE_NEWMAIL _("You have new mail")
#define GGN_MANAGER_TITLE_EMPTY _("No new messages")
#define GGN_MANAGER_SUMMARY_NOACCTS _("Please add one or more accounts \
using the Preferences window.")

/**
 * ggn_manager_check_accounts:
 * @manager: Our #GgnManager object.
 *
 * This is a callable function that initiates account checking.
 *
 * Returns: void.
 **/
void ggn_manager_check_accounts (GgnManager *manager) {
  /* declare a helping variable. */
  static gboolean warned_noaccounts;

  /* are we already checking? */
  if (ggn_account_list_is_checking (manager->priv->alist))
    return;

  /* do we have any accounts to check? */
  if (ggn_account_list_get_num_enabled (manager->priv->alist) > 0) {
    /* set the notification style. */
    ggn_icon_set_style (manager->priv->icon, GGN_ICON_STYLE_CHECKING);

    /* set the notification tooltip. */
    ggn_icon_set_tooltip (manager->priv->icon, GGN_MANAGER_TOOLTIP_CHECKING);

    /* begin checking for new mail. */
    ggn_account_list_check (manager->priv->alist);
  }
  else {
    /* have we warned about this already? */
    if (!warned_noaccounts) {
      /* set the notification style. */
      ggn_icon_set_style (manager->priv->icon, GGN_ICON_STYLE_WARNING);

      /* set the notification tooltip, title and summary. */
      ggn_icon_set_tooltip (manager->priv->icon, GGN_MANAGER_TOOLTIP_NOACCTS);
      ggn_icon_set_title (manager->priv->icon, GGN_MANAGER_TITLE_NOACCTS);
      ggn_icon_set_summary (manager->priv->icon, GGN_MANAGER_SUMMARY_NOACCTS);

      /* show the notification. */
      ggn_icon_show_notification (manager->priv->icon);

      /* we have warned now. */
      warned_noaccounts = TRUE;
    }
  }
}

/**
 * ggn_manager_timeout:
 * @data: The application manager pointer.
 *
 * This function is a callback that is executed every so often by the
 * glib subsystem in order to check the available inboxes for new mail.
 *
 * Returns: FALSE.
 **/
gboolean ggn_manager_timeout (gpointer data) {
  /* declare a helping variable. */
  gint rate;

  /* gain a reference to the manager. */
  GgnManager *manager = GGN_MANAGER (data);

  /* check the accounts. */
  ggn_manager_check_accounts (manager);

  /* register another timeout with the (possibly new) check rate. */
  rate = ggn_prefs_get_int (manager->priv->prefs, GGN_PREF_CHECKRATE);
  g_timeout_add (rate * 60 * 1e3, ggn_manager_timeout, manager);

  /* exit the function. */
  return FALSE;
}

/**
 * ggn_manager_accounts_checked:
 * @alist: The account list that was checked.
 * @data: The application manager pointer.
 *
 * This function is called when a managed #GgnAccount emits its "checked"
 * signal through its managing #GgnAccountList object.
 *
 * Returns: void.
 **/
static void ggn_manager_accounts_checked (GgnAccountList *alist,
                                          gpointer data) {
  /* enter the gdk main thread. */
  gdk_threads_enter ();

  /* declare some helping variables. */
  guint num_unread, tot_unread;
  guint num_new, tot_new;
  guint num_nonempty;
  guint idx, jdx, num;
  GString *gtip, *gttl, *gsum;
  GgnAccount *account;
  GgnEntry *entry;

  /* gain a reference to the manager object. */
  GgnManager *manager = GGN_MANAGER (data);

  /* retrieve the count values. */
  num_nonempty = ggn_account_list_get_num_nonempty (alist);

  /* retrieve the total number of entries. */
  ggn_account_list_get_num_entries (alist, &tot_unread, &tot_new);

  /* act on the count values. */
  if (tot_new == 0 && tot_unread == 0) {
    /* no mail. reset all graphical details. */
    ggn_icon_set_style (manager->priv->icon, GGN_ICON_STYLE_NORMAL);
    ggn_icon_set_tooltip (manager->priv->icon, GGN_MANAGER_TOOLTIP_EMPTY);
  }
  else {
    /* new messages. set the icon style as such. */
    ggn_icon_set_style (manager->priv->icon, GGN_ICON_STYLE_MESSAGE);

    /* allocate the tooltip gstring. */
    gtip = g_string_new ("");

    /* loop through the accounts to build a tooltip. */
    num = 0;
    for (idx = 0; idx < ggn_account_list_length (alist); idx++) {
      /* get the indexed account. */
      account = ggn_account_list_get (alist, idx);

      /* get the entry counts. */
      ggn_account_get_num_entries (account, &num_unread, &num_new);

      /* are there unread messages? */
      if (num_unread + num_new > 0 && ggn_account_get_enabled (account)) {
        /* increment the count variable. */
        num++;

        /* add information to the tooltip. */
        g_string_append_printf (gtip, _("%d unread for %s"),
                                num_unread + num_new,
                                ggn_account_get_desc (account));

        /* add a newline if we need one. */
        if (num < num_nonempty)
          g_string_append_printf (gtip, "\n");
      }
    }

    /* apply the new message tooltip text to the icon. */
    ggn_icon_set_tooltip (manager->priv->icon, gtip->str);

    /* is there new mail, or not? */
    if (tot_new > 0) {
      /* does the user want graphical notification? */
      if (ggn_prefs_get_bool (manager->priv->prefs, GGN_PREF_NOTIFY_NEW)) {
        /* allocate the title and summary gstrings. */
        gttl = g_string_new ("");
        gsum = g_string_new ("");

        /* determine the correct string values. */
        if (tot_new == 1) {
          /* loop until we find the account containing the entry. */
          for (idx = 0; idx < ggn_account_list_length (alist); idx++) {
            /* gain a reference to the indexed account. */
            account = ggn_account_list_get (alist, idx);

            /* get the entry counts from the account. */
            ggn_account_get_num_entries (account, &num_unread, &num_new);

            /* does this account have any entries? */
            if (num_new > 0) {
              /* loop through the entries. */
              for (jdx = 0; jdx < num_unread + num_new; jdx++) {
                /* gain a reference to the entry. */
                entry = ggn_account_get_entry (account, jdx);

                /* is this a new entry? */
                if (ggn_entry_get_state (entry) == GGN_ENTRY_STATE_NEW) {
                  /* use the advanced title gstring. */
                  g_string_printf (gttl, _("New mail from %s"),
                                   ggn_entry_get_sender_name (entry));

                  /* use the single-message summary gstring. */
                  g_string_printf (gsum, "%s", ggn_entry_get_title (entry));

                  /* break from the loop. */
                  break;
                }
              }
            }
          }
        }
        else {
          /* use the basic title gstring. */
          g_string_printf (gttl, GGN_MANAGER_TITLE_NEWMAIL);

          /* add the first part of the summary gstring. */
          g_string_printf (gsum, _("There are <b>%d</b> new messages for"),
                           tot_new);

          /* loop through the accounts list. */
          num = 0;
          for (idx = 0; idx < ggn_account_list_length (alist); idx++) {
            /* gain a reference to the indexed account. */
            account = ggn_account_list_get (alist, idx);

            /* get the entry counts for the account. */
            ggn_account_get_num_entries (account, &num_unread, &num_new);

            /* does this account have any entries? */
            if (num_unread + num_new > 0) {
              /* increment the counter. */
              num++;

              /* where is this account in the list? */
              if (num == 1) {
                /* first. */
                g_string_append_printf (gsum, " %s",
                                        ggn_account_get_desc (account));
              }
              else if (num == num_nonempty) {
                /* last. */
                g_string_append_printf (gsum, " %s %s", _("and"),
                                        ggn_account_get_desc (account));
              }
              else {
                /* anywhere else. */
                g_string_append_printf (gsum, ", %s",
                                        ggn_account_get_desc (account));
              }
            }
          }

          /* do we have between two and four messages? */
          if (tot_new < 5) {
            /* loop through the accounts list. */
            num = 0;
            for (idx = 0; idx < ggn_account_list_length (alist); idx++) {
              /* gain a reference to the indexed account. */
              account = ggn_account_list_get (alist, idx);

              /* get the entry counts for this account. */
              ggn_account_get_num_entries (account, &num_unread, &num_new);

              /* loop through this account's entries. */
              for (jdx = 0; jdx < num_unread + num_new; jdx++) {
                /* gain a reference to the current entry. */
                entry = ggn_account_get_entry (account, jdx);

                /* is this entry one of the new entries? */
                if (ggn_entry_get_state (entry) == GGN_ENTRY_STATE_NEW) {
                  /* increment the counter. */
                  num++;

                  /* where is this entry in the list? */
                  if (num == 1) {
                    /* first. */
                    g_string_append_printf (gsum, " %s %s", _("from"),
                                            ggn_entry_get_sender_name (entry));
                  }
                  else if (num == tot_new) {
                    /* last. */
                    g_string_append_printf (gsum, " %s %s", _("and"),
                                            ggn_entry_get_sender_name (entry));
                  }
                  else {
                    /* anywhere else. */
                    g_string_append_printf (gsum, ", %s",
                                            ggn_entry_get_sender_name (entry));
                  }
                }
              }
            }
          }
        }

        /* apply the new message title text to the icon. */
        ggn_icon_set_title (manager->priv->icon, gttl->str);

        /* apply the new message summary text to the icon. */
        ggn_icon_set_summary (manager->priv->icon, gsum->str);

        /* show the new message notification. */
        ggn_icon_show_notification (manager->priv->icon);

        /* free the allocated gstrings. */
        g_string_free (gttl, TRUE);
        g_string_free (gsum, TRUE);
      }

      /* does the user want audible notification? */
      if (ggn_prefs_get_bool (manager->priv->prefs, GGN_PREF_SOUND_ENAB)) {
        ggn_audio_play_file (manager->priv->audio,
                             ggn_prefs_get_string (manager->priv->prefs,
                                                   GGN_PREF_SOUND_FILE));
      }
    }

    /* free the tooltip gstring. */
    g_string_free (gtip, TRUE);
  }

  /* leave the gdk main thread. */
  gdk_threads_leave ();
}

/**
 * ggn_manager_accounts_failed:
 * @alist: The account list that failed.
 * @data: The application manager pointer.
 *
 * This function is called when a managed #GgnAccount emits its "failed"
 * signal through its managing #GgnAccountList object.
 *
 * Returns: void.
 **/
static void ggn_manager_accounts_failed (GgnAccountList *alist,
                                         gpointer data) {
  /* enter the gdk main thread. */
  gdk_threads_enter ();

  /* declare a few helping variables. */
  guint idx, errors, tot_errors;
  static gboolean notified;
  GgnAccount *account;
  GString *gsum;

  /* gain a reference to the manager object. */
  GgnManager *manager = GGN_MANAGER (data);

  /* initialize the counting variables. */
  errors = 0;

  /* set the icon style accordingly. */
  ggn_icon_set_style (manager->priv->icon, GGN_ICON_STYLE_ERROR);

  /* does the user want graphical notification? */
  if (ggn_prefs_get_bool (manager->priv->prefs, GGN_PREF_NOTIFY_ERR) &&
      notified == FALSE) {
    /* get the number of errors. */
    tot_errors = ggn_account_list_get_num_failed (alist);

    /* set the icon tooltip. */
    ggn_icon_set_tooltip (manager->priv->icon, GGN_MANAGER_TOOLTIP_FAILURE);

    /* set the notification title. */
    ggn_icon_set_title (manager->priv->icon, GGN_MANAGER_TITLE_FAILURE);

    /* allocate a new summary gstring. */
    gsum = g_string_new (_("Errors occurred while checking mail for"));

    /* loop through the failed accounts. */
    for (idx = 0; idx < ggn_account_list_length (alist); idx++) {
      /* gain a reference to the indexed account. */
      account = ggn_account_list_get (alist, idx);

      /* does this account have errors? */
      if (ggn_account_get_state (account) == GGN_ACCOUNT_STATE_FAILED) {
        /* increment the error counter. */
        errors++;

        /* see where in the list of failed accounts we are. */
        if (errors == 1) {
          /* first. */
          g_string_append_printf (gsum, " %s",
                                  ggn_account_get_desc (account));
        }
        else if (errors == tot_errors) {
          /* last. */
          g_string_append_printf (gsum, " %s %s", _("and"),
                                  ggn_account_get_desc (account));
        }
        else {
          /* anywhere else. */
          g_string_append_printf (gsum, ", %s",
                                  ggn_account_get_desc (account));
        }
      }
    }

    /* set the notification summary from the gstring contents. */
    ggn_icon_set_summary (manager->priv->icon, gsum->str);

    /* show the error notification. */
    ggn_icon_show_notification (manager->priv->icon);

    /* we have notified the user once. no more. */
    notified = TRUE;

    /* free the summary gstring. */
    g_string_free (gsum, TRUE);
  }

  /* leave the gdk main thread. */
  gdk_threads_leave ();
}

/* explicitly predefine the extended menu builder function. */
static void ggn_manager_build_extended_menu (GgnManager *manager);

/**
 * ggn_manager_icon_clicked:
 * @icon: The status icon that was used.
 * @data: The application manager pointer.
 *
 * This function is called when the manager's #GgnIcon has been clicked.
 *
 * Returns: void.
 **/
static void ggn_manager_icon_clicked (GgnIcon *icon, gpointer data) {
  /* declare some helping variables. */
  GgnAccount *account;
  guint idx, num_enabled;

  /* gain a reference to the manager object. */
  GgnManager *manager = GGN_MANAGER (data);

  /* get the number of enabled accounts. */
  num_enabled = ggn_account_list_get_num_enabled (manager->priv->alist);

  /* how many accounts do we have? */
  if (num_enabled == 0) {
    /* just fail. */
    return;
  }
  else if (num_enabled == 1) {
    /* find the one enabled account. */
    for (idx = 0; idx < ggn_account_list_length (manager->priv->alist); idx++) {
      /* gain a reference to the account. */
      account = ggn_account_list_get (manager->priv->alist, idx);

      /* is this account enabled? */
      if (ggn_account_get_enabled (account)) {
        /* launch the inbox for that account. */
        ggn_account_launch_inbox (account);

        /* end the loop. */
        break;
      }
    }
  }
  else {
    /* has the extended menu been allocated? */
    if (icon->priv->exmenu)
      gtk_widget_destroy (GTK_WIDGET (icon->priv->exmenu));

    /* re-allocate the extended menu. */
    icon->priv->exmenu = (GtkMenu *) gtk_menu_new ();

    /* build the extended menu using managed information. */
    ggn_manager_build_extended_menu (manager);

    /* show the extended menu. */
    gtk_menu_popup (icon->priv->exmenu, NULL, NULL, 
                    gtk_status_icon_position_menu,
                    icon->priv->icon, 0,
                    gtk_get_current_event_time ());
  }
}

/**
 * ggn_manager_menu_account_selected:
 * @icon: The #GtkMenuItem that was used.
 * @data: The application manager pointer.
 *
 * This function is called when the user clicks on an account in
 * the extended left-click menu in order to open its inbox.
 *
 * Returns: void.
 **/
static void ggn_manager_menu_account_selected (GtkMenuItem *item,
                                               gpointer data) {
  /* gain a reference to the account. */
  GgnAccount *account = GGN_ACCOUNT (data);

  /* launch the inbox of that account. */
  ggn_account_launch_inbox (account);
}

/**
 * ggn_manager_menu_compose_selected:
 * @icon: The #GtkMenuItem that was used.
 * @data: The application manager pointer.
 *
 * This function is called when the user clicks on the compose
 * option in the extended left-click menu.
 *
 * Returns: void.
 **/
static void ggn_manager_menu_compose_selected (GtkMenuItem *item,
                                               gpointer data) {
  /* declare a helping variable. */
  GgnAccount *account;

  /* gain a reference to the manager. */
  GgnManager *manager = GGN_MANAGER (data);

  /* get the default account. */
  account = ggn_account_list_get_default (manager->priv->alist);

  /* launch the composition browser window. */
  if (account)
    ggn_account_launch_compose (account, "");
}

/**
 * ggn_manager_icon_check_selected:
 * @icon: The status icon that was used.
 * @data: The application manager pointer.
 *
 * This function is called when the manager's #GgnIcon has been used.
 *
 * Returns: void.
 **/
static void ggn_manager_icon_check_selected (GgnIcon *icon, gpointer data) {
  /* gain a reference to the manager. */
  GgnManager *manager = GGN_MANAGER (data);

  /* try to check for mail. */
  ggn_manager_check_accounts (manager);
}

/**
 * ggn_manager_icon_prefs_selected:
 * @icon: The status icon that was used.
 * @data: The application manager pointer.
 *
 * This function is called when the manager's #GgnIcon has been used.
 *
 * Returns: void.
 **/
static void ggn_manager_icon_prefs_selected (GgnIcon *icon, gpointer data) {
  /* gain a reference to our manager. */
  GgnManager *manager = GGN_MANAGER (data);

  /* show the preferences window. */
  ggn_prefs_window_show (manager->priv->prefs_win);
}

/**
 * ggn_manager_icon_about_selected:
 * @icon: The status icon that was used.
 * @data: The application manager pointer.
 *
 * This function is called when the manager's #GgnIcon has been used.
 *
 * Returns: void.
 **/
static void ggn_manager_icon_about_selected (GgnIcon *icon, gpointer data) {
  /* gain a reference to our manager. */
  GgnManager *manager = GGN_MANAGER (data);

  /* show the about dialog. */
  ggn_about_dialog_show (manager->priv->about_dlg);
}

/**
 * ggn_manager_icon_quit_selected:
 * @icon: The status icon that was used.
 * @data: The application manager pointer.
 *
 * This function is called when the manager's #GgnIcon has been used.
 *
 * Returns: void.
 **/
static void ggn_manager_icon_quit_selected (GgnIcon *icon, gpointer data) {
  /* gain a reference to our manager. */
  GgnManager *manager = GGN_MANAGER (data);

  /* quit the application, gracefully. */
  ggn_manager_quit (manager);
}

/**
 * ggn_manager_build_account_menu_item:
 * @account: The #GgnAccount to access for information.
 *
 * This function builds a #GtkMenuItem for a single account in the
 * extended left-click menu for the notifier.
 *
 * Returns: A newly allocated #GtkMenuItem.
 **/
static GtkWidget *ggn_manager_build_account_menu_item (GgnManager *manager,
                                                       GgnAccount *account) {
  /* declare some helping variables. */
  GtkWidget *item, *hbox, *image, *lname, *linfo;
  guint num_unread, num_new, state;
  gchar *desc, *markup;

  /* get the entry counts and details for the account. */
  ggn_account_get_num_entries (account, &num_unread, &num_new);
  state = ggn_account_get_state (account);
  desc = ggn_account_get_desc (account);

  /* create a menu item */
  item = gtk_menu_item_new ();

  /* create a horizontal box. */
  hbox = gtk_hbox_new (FALSE, 0);

  /* create the info and name labels. */
  linfo = gtk_label_new (NULL);
  lname = gtk_label_new (NULL);

  /* FIXME: figure out how to resize menu (*-sm,write) icons to 22x22 size. */
  /* FIXME: rework all SVGs from basis: write, error-sm, new-sm, normal-sm. */

  /* determine account information. */
  if (state == GGN_ACCOUNT_STATE_FAILED) {
    /* show the failure emblem. */
    image = gtk_image_new_from_file (GGN_PIXMAP_ERROR_SM);

    /* set the markup for the info label. */
    markup = g_strdup_printf ("<span style=\"%s\" color=\"%s\">- %s</span>",
                              "italic", "#888888", GGN_MANAGER_TITLE_FAILURE);
  }
  else if (num_unread + num_new > 0) {
    /* show the new message emblem. */
    image = gtk_image_new_from_file (GGN_PIXMAP_MESSAGE_SM);

    /* set the markup for the info label. */
    markup = g_strdup_printf ("<span style=\"%s\" color=\"%s\">- %d %s</span>",
                              "italic", "#888888", num_unread + num_new,
                              _("unread"));
  }
  else {
    /* as a catch-all, show the no messages emblem. */
    image = gtk_image_new_from_file (GGN_PIXMAP_NORMAL_SM);

    /* set the markup for the info label. */
    markup = g_strdup_printf ("<span style=\"%s\" color=\"%s\">- %s</span>",
                              "italic", "#888888", GGN_MANAGER_TITLE_EMPTY);
  }

  /* apply the info label markup and align it to the left. */
  gtk_label_set_markup (GTK_LABEL (linfo), markup);
  g_object_set (G_OBJECT (linfo), "xalign", 0.0, NULL);
  g_free (markup);

  /* build and apply the name label markup. */
  markup = g_strdup_printf ("<b>%s</b>", desc);
  gtk_label_set_markup (GTK_LABEL (lname), markup);
  g_free (markup);

  /* pack the widgets into the horizontal box. */
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 4);
  gtk_box_pack_start (GTK_BOX (hbox), lname, FALSE, FALSE, 4);
  gtk_box_pack_start (GTK_BOX (hbox), linfo, TRUE, TRUE, 0);

  /* pack the box into the menu item. */
  gtk_container_add (GTK_CONTAINER (item), hbox);

  /* register the account menu item with its callback. */
  g_signal_connect (G_OBJECT (item),
                    "activate",
                    G_CALLBACK (ggn_manager_menu_account_selected),
                    account);

  /* return the final menu item. */
  return item;
}

/**
 * ggn_manager_build_compose_menu_item:
 * @account: The #GgnAccount to access for information.
 *
 * This function builds a #GtkMenuItem for a single account in the
 * extended left-click menu for the notifier.
 *
 * Returns: A newly allocated #GtkMenuItem.
 **/
static GtkWidget *ggn_manager_build_compose_menu_item (GgnManager *manager) {
  /* declare some helping variables. */
  GtkWidget *item, *hbox, *image, *label;

  /* create the menu item. */
  item = gtk_menu_item_new ();

  /* create the horizontal box. */
  hbox = gtk_hbox_new (FALSE, 0);

  /* create the image. */
  image = gtk_image_new_from_file (GGN_PIXMAP_COMPOSE);

  /* create the label. */
  label = gtk_label_new (_("Compose New Message"));
  g_object_set (G_OBJECT (label), "xalign", 0.0, NULL);

  /* pack the widgets into the horizontal box. */
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 4);
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 4);

  /* pack the box into the menu item. */
  gtk_container_add (GTK_CONTAINER (item), hbox);

  /* register the compose menu item with its callback. */
  g_signal_connect (G_OBJECT (item),
                    "activate",
                    G_CALLBACK (ggn_manager_menu_compose_selected),
                    manager);

  /* return the newly created menu item. */
  return item;
}

/**
 * ggn_manager_build_extended_menu:
 * @manager: The #GgnManager to use.
 *
 * This function builds the extended menu for the icon to display
 * when the user left-clicks the status icon under conditions of
 * multiple enabled accounts.
 *
 * Returns: void.
 **/
static void ggn_manager_build_extended_menu (GgnManager *manager) {
  /* declare some helping variables. */
  GgnAccount *account;
  GtkWidget *item;
  guint idx;

  /* loop for each account in the list. */
  for (idx = 0; idx < ggn_account_list_length (manager->priv->alist); idx++) {
    /* gain a reference to the indexed account. */
    account = ggn_account_list_get (manager->priv->alist, idx);

    /* skip disabled accounts. */
    if (!ggn_account_get_enabled (account))
      continue;

    /* build and append the menu item onto the extended menu. */
    item = ggn_manager_build_account_menu_item (manager, account);
    gtk_menu_shell_append (GTK_MENU_SHELL (manager->priv->icon->priv->exmenu),
                           item);
  }

  /* add a separator menu item. */
  item = gtk_separator_menu_item_new ();
  gtk_menu_shell_append (GTK_MENU_SHELL (manager->priv->icon->priv->exmenu),
                         item);

  /* add a compose menu item. */
  item = ggn_manager_build_compose_menu_item (manager);
  gtk_menu_shell_append (GTK_MENU_SHELL (manager->priv->icon->priv->exmenu),
                         item);

  /* show the menu and all its menu items. */
  gtk_widget_show_all (GTK_WIDGET (manager->priv->icon->priv->exmenu));
}

/*
 * ggn_manager_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_manager_init (GgnManager *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnManagerPrivate, 1);

  /* create the globally used prefs object. */
  self->priv->prefs = ggn_prefs_new ();

  /* build the managed accounts list from the prefs object. */
  self->priv->alist = ggn_account_list_new_from_prefs (self->priv->prefs);

  /* connect to the account list's "checked" signal. */
  g_signal_connect (G_OBJECT (self->priv->alist),
                    "checked",
                    G_CALLBACK (ggn_manager_accounts_checked),
                    self);

  /* connect to the account list's "failed" signal. */
  g_signal_connect (G_OBJECT (self->priv->alist),
                    "failed",
                    G_CALLBACK (ggn_manager_accounts_failed),
                    self);

  /* create the status icon. */
  self->priv->icon = ggn_icon_new ();

  /* connect to the status icon's "clicked" signal. */
  g_signal_connect (G_OBJECT (self->priv->icon),
                    "clicked",
                    G_CALLBACK (ggn_manager_icon_clicked),
                    self);

  /* connect to the status icon's "check_selected" signal. */
  g_signal_connect (G_OBJECT (self->priv->icon),
                    "check_selected",
                    G_CALLBACK (ggn_manager_icon_check_selected),
                    self);

  /* connect to the status icon's "prefs_selected" signal. */
  g_signal_connect (G_OBJECT (self->priv->icon),
                    "prefs_selected",
                    G_CALLBACK (ggn_manager_icon_prefs_selected),
                    self);

  /* connect to the status icon's "about_selected" signal. */
  g_signal_connect (G_OBJECT (self->priv->icon),
                    "about_selected",
                    G_CALLBACK (ggn_manager_icon_about_selected),
                    self);

  /* connect to the status icon's "quit_selected" signal. */
  g_signal_connect (G_OBJECT (self->priv->icon),
                    "quit_selected",
                    G_CALLBACK (ggn_manager_icon_quit_selected),
                    self);

  /* create the audio object. */
  self->priv->audio = ggn_audio_new ();

  /* create the about dialog. */
  self->priv->about_dlg = ggn_about_dialog_new ();

  /* create the preferences window. */
  self->priv->prefs_win = ggn_prefs_window_new_from_manager (self);
}

/*
 * ggn_manager_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_manager_class_init (GgnManagerClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_manager_finalize;
}

/*
 * ggn_manager_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_manager_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnManager *self = GGN_MANAGER (obj);

  /* free the preferences window. */
  ggn_prefs_window_free (self->priv->prefs_win);

  /* free the about dialog. */
  ggn_about_dialog_free (self->priv->about_dlg);

  /* free the status icon. */
  ggn_icon_free (self->priv->icon);

  /* free the account list object. */
  ggn_account_list_free (self->priv->alist);

  /* free the prefs object. */
  ggn_prefs_free (self->priv->prefs);

  /* free the audio playback object. */
  ggn_audio_free (self->priv->audio);

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_manager_parent_class)->finalize (obj);
}

/**
 * ggn_manager_new:
 *
 * Creates a new #GgnManager with default values, which are
 * used to play coordinate the upper-level functions of the
 * notifier. Essentially the main function hands off control
 * to the GgnManager, which starts up all the required things.
 *
 * Returns: the new #GgnManager object. Free with ggn_manager_free().
 **/
GgnManager *ggn_manager_new (void) {
  /* make a newly created gobject. */
  GgnManager *manager = g_object_new (GGN_TYPE_MANAGER, NULL);

  /* return the new object. */
  return manager;
}

/**
 * ggn_manager_free:
 * @manager: The #GgnManager to free.
 *
 * Frees the given #GgnManager object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_manager_free (GgnManager *manager) {
  /* while this object exists... */
  while (G_IS_OBJECT (manager)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (manager));
  }
}

/**
 * ggn_manager_main:
 * @manager: The #GgnManager to start.
 * @argc: The number of passed commandline arguments.
 * @argv: The array of passed commandline arguments.
 *
 * This function sets the objects controlled by the #GgnManager into
 * motion, making it essentially like calling gtk_main() for a GTK+
 * application.
 *
 * Returns: void.
 **/
void ggn_manager_main (GgnManager *manager, int argc, char **argv) {
  /* declare some helping variables. */
  GgnAccount *account;
  gchar **email;
  gint rate;

  /* has the user passed the notifier an email address? */
  if (argc == 2) {
    /* get the email string. */
    email = g_strsplit (argv[1], ":", 2);

    /* ensure correct formatting. */
    if (g_utf8_collate (email[0], "mailto") != 0 || !email[1])
      return;

    /* ok, we have a correct email. get the default account. */
    account = ggn_account_list_get_default (manager->priv->alist);

    /* launch the composition browser window. */
    if (account)
      ggn_account_launch_compose (account, email[1]);

    /* free the string array. */
    g_strfreev (email);

    /* quit. */
    return;
  }

  /* set the default style of the notification icon. */
  ggn_icon_set_style (manager->priv->icon, GGN_ICON_STYLE_NORMAL);
  ggn_icon_show (manager->priv->icon);

  /* try to check for new mail. */
  ggn_manager_check_accounts (manager);

  /* set the inbox checking loop into motion. */
  rate = ggn_prefs_get_int (manager->priv->prefs, GGN_PREF_CHECKRATE);
  g_timeout_add (rate * 60 * 1e3, ggn_manager_timeout, manager);

  /* start the main loop. */
  gtk_main ();
}

/**
 * ggn_manager_quit:
 * @manager: The #GgnManager to stop.
 *
 * This function stops a running, allocated #GgnManager object. Given
 * the fact that the #GgnManager is the crux of the application, this
 * function also ends program execution after cleaning up and such.
 *
 * Returns: void.
 **/
void ggn_manager_quit (GgnManager *manager) {
  /* hide the prefs window and about dialog. */
  ggn_prefs_window_hide (manager->priv->prefs_win);
  ggn_about_dialog_hide (manager->priv->about_dlg);

  /* hide the status icon. */
  ggn_icon_hide (manager->priv->icon);

  /* free the manager. */
  ggn_manager_free (manager);

  /* end the main loop. */
  gtk_main_quit ();
}

