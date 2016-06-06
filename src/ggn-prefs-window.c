/**
 * @file src/ggn-prefs-window.c The GgnPrefsWindow object
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
 * SECTION:GgnPrefsWindow
 * @short_description: The preferences window object
 * @include: src/ggn-prefs-window.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-prefs-window-priv.h"
#include "ggn-manager-priv.h"

/*
 * forward function definitions.
 */
static void ggn_prefs_window_init (GgnPrefsWindow *self);
static void ggn_prefs_window_class_init (GgnPrefsWindowClass *klass);
static void ggn_prefs_window_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnPrefsWindow, ggn_prefs_window, G_TYPE_OBJECT);

/**
 * ggn_prefs_window_show_editor:
 * @window: The #GgnPrefsWindow we're working with.
 *
 * A utility function which is used to show a useful account editor
 * window in the event that one is required by the user's actions.
 *
 * Returns: void.
 **/
void ggn_prefs_window_show_editor (GgnPrefsWindow *window) {
  /* set the textual contents of the description entry. */
  gtk_entry_set_text (window->priv->txt_edit_desc,
                      ggn_account_get_desc (window->priv->editing));

  /* set the textual contents of the username entry. */
  gtk_entry_set_text (window->priv->txt_edit_user,
                      ggn_account_get_user (window->priv->editing));

  /* set the textual contents of the domain name entry. */
  gtk_entry_set_text (window->priv->txt_edit_domain,
                      ggn_account_get_domain (window->priv->editing));

  /* set the textual contents of the password entry. */
  gtk_entry_set_text (window->priv->txt_edit_pass,
                      ggn_account_get_pass (window->priv->editing));

  /* set the boolean value of the default checkbutton widget. */
  gtk_toggle_button_set_active (window->priv->chk_edit_def,
                                ggn_account_get_default
                                  (window->priv->editing));

  /* set the sensitive value of the default checkbutton widget. */
  gtk_widget_set_sensitive (GTK_WIDGET (window->priv->chk_edit_def),
                            !ggn_account_get_default (window->priv->editing));

  /* show the editor window. */
  gtk_widget_show (GTK_WIDGET (window->priv->win_edit));
}

/**
 * ggn_prefs_window_reload_listview:
 * @window: The #GgnPrefsWindow we're working with.
 *
 * A utility function which is used to reload the contents of the
 * listview holding the account listing.
 *
 * Returns: void.
 **/
void ggn_prefs_window_reload_listview (GgnPrefsWindow *window) {
  /* declare some helping variables. */
  GgnAccount *account;
  gboolean enab;
  gchar *email;
  gchar *desc;
  guint idx;

  /* clear the list store. */
  gtk_list_store_clear (window->priv->list);

  /* loop through the accounts list. */
  for (idx = 0; idx < ggn_account_list_length (window->priv->alist); idx++) {
    /* get the account at the current index. */
    account = ggn_account_list_get (window->priv->alist, idx);

    /* construct the email string. */
    email = ggn_account_get_email (account);

    /* get the description string. */
    desc = ggn_account_get_desc (account);

    /* get the enabled value. */
    enab = ggn_account_get_enabled (account);

    /* append a new row object. */
    gtk_list_store_append (window->priv->list, &window->priv->iter);

    /* set the values in the list store. */
    gtk_list_store_set (window->priv->list,
                        &window->priv->iter,
                        ACCOUNT_ENAB, enab,
                        ACCOUNT_DESC, desc,
                        ACCOUNT_EMAIL, email,
                        -1);

    /* free the allocated strings. (desc is an in-use string!!) */
    g_free (email);
  }
}

/**
 * ggn_prefs_window_deleted:
 * @widget: The #GtkWidget that emitted the signal.
 * @event: The event which occurred in the widget.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the
 * close button in the window border of the preferences window.
 *
 * Returns: success #gboolean.
 **/
G_MODULE_EXPORT
gboolean ggn_prefs_window_deleted (GtkWidget *widget,
                                   GdkEvent *event,
                                   gpointer data) {
  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* hide the preferences window. */
  ggn_prefs_window_hide (window);

  /* we handled this signal. */
  return TRUE;
}

/**
 * ggn_prefs_window_key_pressed:
 * @widget: The #GtkWidget that emitted the signal.
 * @event: The event which occurred in the widget.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user presses a key
 * with the preferences window in focus, allowing us to scan for
 * the escape keycode and close the window.
 *
 * Returns: success #gboolean.
 **/
G_MODULE_EXPORT
gboolean ggn_prefs_window_key_pressed (GtkWidget *widget,
                                       GdkEventKey *event,
                                       gpointer data) {
  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* check for the escape keypress. */
  if (event->keyval == GDK_Escape) {
    /* close the window. */
    ggn_prefs_window_hide (window);
  }

  /* this callback was handled. */
  return TRUE;
}

/**
 * ggn_prefs_window_account_toggled:
 * @renderer: The #GtkCellRendererToggle that was toggled.
 * @path: The string path of the cell.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user toggles an account
 * in the accounts list view, which either enables or disables the
 * given account.
 *
 * Returns: void.
 **/
static void ggn_prefs_window_account_toggled (GtkCellRendererToggle *renderer,
                                              gchar *path,
                                              gpointer data) {
  /* declare some helping variables. */
  GtkTreePath *treepath;
  gchar *email;
  gint idx;

  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* get the tree path. */
  treepath = gtk_tree_path_new_from_string (path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL (window->priv->list),
                           &window->priv->iter, treepath);

  /* get the tree path iterator. */
  gtk_tree_model_get (GTK_TREE_MODEL (window->priv->list),
                      &window->priv->iter,
                      ACCOUNT_EMAIL,
                      &email, -1);

  /* try to find the matching account. */
  idx = ggn_account_list_search (window->priv->alist, email);

  /* if we found it... */
  if (idx >= 0) {
    /* get the account object. */
    window->priv->editing = ggn_account_list_get (window->priv->alist, idx);

    /* toggle the enabled value. */
    ggn_account_set_enabled (window->priv->editing,
                             !ggn_account_get_enabled (window->priv->editing));

    /* set the new value in the listview. */
    gtk_list_store_set (window->priv->list,
                        &window->priv->iter,
                        ACCOUNT_ENAB,
                        ggn_account_get_enabled (window->priv->editing),
                        -1);
  }

  /* free the email string. */
  g_free (email);

  /* free the tree path. */
  gtk_tree_path_free (treepath);
}

/**
 * ggn_prefs_window_account_selected:
 * @sel: The #GtkTreeSelection that was used.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user selects an account
 * in the accounts list view.
 *
 * Returns: void.
 **/
static void ggn_prefs_window_account_selected (GtkTreeSelection *sel,
                                               gpointer data) {
  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* get the account edit button. */
  GtkWidget *btn_edit = GTK_WIDGET (gtk_builder_get_object
                                      (window->priv->xml_prefs,
                                       "btnAccountEdit"));

  /* get the account delete button. */
  GtkWidget *btn_del = GTK_WIDGET (gtk_builder_get_object
                                     (window->priv->xml_prefs,
                                      "btnAccountDel"));

  /* get the tree model of the list view. */
  GtkTreeModel *model = GTK_TREE_MODEL (window->priv->list);

  /* act on the selection. */
  if (gtk_tree_selection_get_selected (sel, &model, &window->priv->iter)) {
    /* we have something selected, enable the buttons. */
    gtk_widget_set_sensitive (btn_edit, TRUE);
    gtk_widget_set_sensitive (btn_del, TRUE);
  }
  else {
    /* nothing is selected, disable the buttons. */
    gtk_widget_set_sensitive (btn_edit, FALSE);
    gtk_widget_set_sensitive (btn_del, FALSE);
  }
}

/**
 * ggn_prefs_window_account_activated:
 * @treeview: The #GtkTreeView that was activated.
 * @path: The #GtkTreePath to the activated row.
 * @column: The activated column.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user double-clicks an account
 * in the accounts list view, indicating it should be brought up for editing.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_account_activated (GtkTreeView *treeview,
                                         GtkTreePath *path,
                                         GtkTreeViewColumn *column,
                                         gpointer data) {
  /* declare some helping variables. */
  GtkTreeModel *model;
  GtkTreeView *tree;
  gchar *email;
  gint idx;

  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* get a reference to the tree view. */
  tree = GTK_TREE_VIEW (gtk_builder_get_object (window->priv->xml_prefs,
                                                "treeAccounts"));

  /* get the current listview selection. */
  window->priv->sel = gtk_tree_view_get_selection (tree);

  /* get the iterator. */
  model = GTK_TREE_MODEL (window->priv->list);
  if (!gtk_tree_selection_get_selected (window->priv->sel, &model,
                                        &window->priv->iter)) {
    /* nothing is selected. */
    return;
  }

  /* determine the account email address. */
  gtk_tree_model_get (model, &window->priv->iter, ACCOUNT_EMAIL, &email, -1);

  /* search for the matching account. */
  idx = ggn_account_list_search (window->priv->alist, email);

  /* if the account was found... */
  if (idx >= 0) {
    /* get the account object. */
    window->priv->editing = ggn_account_list_get (window->priv->alist, idx);

    /* show the account editor window. */
    ggn_prefs_window_show_editor (window);
  }

  /* free the email string. */
  g_free (email);
}

/**
 * ggn_prefs_window_account_add:
 * @button: The #GtkButton that was clicked.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the button
 * to add an account to the list view.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_account_add (GtkButton *button, gpointer data) {
  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* create a new account. */
  window->priv->editing = ggn_account_new ();

  /* set the default account values. */
  ggn_account_set_desc (window->priv->editing, _("New Account"));
  ggn_account_set_pass (window->priv->editing, "");
  ggn_account_set_user (window->priv->editing, "username");
  ggn_account_set_domain (window->priv->editing, "gmail.com");
  ggn_account_set_enabled (window->priv->editing, FALSE);
  ggn_account_set_default (window->priv->editing, FALSE);

  /* add the account to the accounts list. */
  ggn_account_list_add (window->priv->alist, window->priv->editing);

  /* update the list view. */
  ggn_prefs_window_reload_listview (window);

  /* show the account editor window. */
  ggn_prefs_window_show_editor (window);
}

/**
 * ggn_prefs_window_account_del:
 * @button: The #GtkButton that was clicked.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the button
 * to delete an account from the list view.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_account_del (GtkButton *button, gpointer data) {
  /* declare some helping variables. */
  GtkTreeModel *model;
  GtkTreeView *tree;
  gchar *email;
  gint idx;

  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* get a reference to the accounts tree view. */
  tree = GTK_TREE_VIEW (gtk_builder_get_object (window->priv->xml_prefs,
                                                "treeAccounts"));

  /* get the current list selection. */
  window->priv->sel = gtk_tree_view_get_selection (tree);

  /* get the iterator. */
  model = GTK_TREE_MODEL (window->priv->list);
  if (!gtk_tree_selection_get_selected (window->priv->sel, &model,
                                        &window->priv->iter)) {
    /* nothing is selected. */
    return;
  }

  /* determine the account email string. */
  gtk_tree_model_get (model, &window->priv->iter, ACCOUNT_EMAIL, &email, -1);

  /* get the matching account index, if any exists. */
  idx = ggn_account_list_search (window->priv->alist, email);

  /* see if a matching account exists. */
  if (idx >= 0) {
    /* remove this account from the list. */
    ggn_account_list_del (window->priv->alist, idx);

    /* update the list view. */
    ggn_prefs_window_reload_listview (window);
  }

  /* free the email string. */
  g_free (email);
}

/**
 * ggn_prefs_window_account_edit:
 * @button: The #GtkButton that was clicked.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the button
 * to edit an account in the list view.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_account_edit (GtkButton *button, gpointer data) {
  /* declare some helping variables. */
  GtkTreeModel *model;
  GtkTreeView *tree;
  gchar *email;
  gint idx;

  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* get a reference to the accounts tree view. */
  tree = GTK_TREE_VIEW (gtk_builder_get_object (window->priv->xml_prefs,
                                                "treeAccounts"));

  /* get the current list selection. */
  window->priv->sel = gtk_tree_view_get_selection (tree);

  /* get the iterator. */
  model = GTK_TREE_MODEL (window->priv->list);
  if (!gtk_tree_selection_get_selected (window->priv->sel, &model,
                                        &window->priv->iter)) {
    /* nothing is selected. */
    return;
  }

  /* determine the account email string. */
  gtk_tree_model_get (model, &window->priv->iter, ACCOUNT_EMAIL, &email, -1);

  /* get the matching account index, if any exists. */
  idx = ggn_account_list_search (window->priv->alist, email);

  /* see if a matching account exists. */
  if (idx >= 0) {
    /* get the account object. */
    window->priv->editing = ggn_account_list_get (window->priv->alist, idx);

    /* show the account editor window. */
    ggn_prefs_window_show_editor (window);
  }

  /* free the email string. */
  g_free (email);
}

/**
 * ggn_prefs_window_rate_changed:
 * @range: The #GtkRange that was changed.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user changes the value of
 * the check rate slider.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_rate_changed (GtkRange *range, gpointer data) {
  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* save the new value to the gconf-2 database. */
  ggn_prefs_set_int (window->priv->prefs,
                     GGN_PREF_CHECKRATE,
                     (gint) gtk_range_get_value (range));
}

/**
 * ggn_prefs_window_rate_format:
 * @scale: The #GtkScale that needs formatting.
 * @value: The value of the scale in question.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user changes the value of
 * the check rate slider to determine what string to print underneath.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
gchar *ggn_prefs_window_rate_format (GtkScale *scale,
                                     gdouble value,
                                     gpointer data) {
  /* look at the value. */
  if (value == 1) {
    /* singular value. */
    return g_strdup_printf ("%1.d %s", (gint) value, _("minute"));
  }
  else if (value <= 9) {
    /* plural value 2-9. */
    return g_strdup_printf ("%1.d %s", (gint) value, _("minutes"));
  }
  else {
    /* plural value 10-60. */
    return g_strdup_printf ("%2.d %s", (gint) value, _("minutes"));
  }
}

/**
 * ggn_prefs_window_notify_msgs_toggled:
 * @button: The #GtkToggleButton that was toggled.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user toggles the message
 * notification bubble check button.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_notify_msgs_toggled (GtkToggleButton *button,
                                           gpointer data) {
  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* set the new value with the prefs object. */
  ggn_prefs_set_bool (window->priv->prefs, GGN_PREF_NOTIFY_NEW,
                      gtk_toggle_button_get_active (button));
}

/**
 * ggn_prefs_window_notify_errs_toggled:
 * @button: The #GtkToggleButton that was toggled.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user toggles the error
 * notification bubble check button.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_notify_errs_toggled (GtkToggleButton *button,
                                           gpointer data) {
  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* set the new value with the prefs object. */
  ggn_prefs_set_bool (window->priv->prefs, GGN_PREF_NOTIFY_ERR,
                      gtk_toggle_button_get_active (button));
}

/**
 * ggn_prefs_window_sound_enab_toggled:
 * @button: The #GtkToggleButton that was toggled.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user toggles the sound
 * enabling check button.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_sound_enab_toggled (GtkToggleButton *button,
                                          gpointer data) {
  /* gain a reference to the preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* set the new value with the prefs object. */
  ggn_prefs_set_bool (window->priv->prefs, GGN_PREF_SOUND_ENAB,
                      gtk_toggle_button_get_active (button));
}

/**
 * ggn_prefs_window_test_sound:
 * @button: The #GtkButton that was clicked.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the button
 * to test the currently selected sound file.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_test_sound (GtkButton *button, gpointer data) {
  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* determine the current filename. */
  gchar *filename = gtk_file_chooser_get_filename
                      (GTK_FILE_CHOOSER (window->priv->chooser));

  /* play the sound file. */
  if (window->priv->audio)
    ggn_audio_play_file (window->priv->audio, filename);

  /* free the filename string. */
  g_free (filename);
}

/**
 * ggn_prefs_window_closed:
 * @button: The #GtkButton that emitted the signal.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the
 * "Close" button in the preferences window.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_prefs_window_closed (GtkButton *button, gpointer data) {
  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* hide the preferences window. */
  ggn_prefs_window_hide (window);
}

/**
 * ggn_edit_window_deleted:
 * @widget: The #GtkWidget that emitted the signal.
 * @event: The event which occurred in the widget.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the
 * close button in the window border of the account editor window.
 *
 * Returns: success #gboolean.
 **/
G_MODULE_EXPORT
gboolean ggn_edit_window_deleted (GtkWidget *widget,
                                  GdkEvent *event,
                                  gpointer data) {
  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* hide the account editor window. */
  gtk_widget_hide (GTK_WIDGET (window->priv->win_edit));

  /* we handled this signal. */
  return TRUE;
}

/**
 * ggn_edit_window_key_pressed:
 * @widget: The #GtkWidget that emitted the signal.
 * @event: The event which occurred in the widget.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user presses a key
 * with the account editor window in focus, allowing us to scan for
 * the escape keycode and close the window.
 *
 * Returns: success #gboolean.
 **/
G_MODULE_EXPORT
gboolean ggn_edit_window_key_pressed (GtkWidget *widget,
                                      GdkEventKey *event,
                                      gpointer data) {
  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* check for the escape keypress. */
  if (event->keyval == GDK_Escape) {
    /* close the window. */
    gtk_widget_hide (GTK_WIDGET (window->priv->win_edit));
  }

  /* this callback is not always the end of the line. */
  return FALSE;
}

/**
 * ggn_edit_window_cancelled:
 * @button: The #GtkButton that emitted the signal.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the
 * "Cancel" button in the account editor window.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_edit_window_cancelled (GtkButton *button, gpointer data) {
  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* hide the editor window. */
  gtk_widget_hide (GTK_WIDGET (window->priv->win_edit));
}

/**
 * ggn_edit_window_confirmed:
 * @button: The #GtkButton that emitted the signal.
 * @data: Private data, in this case our #GgnPrefsWindow.
 *
 * Callback function that is executed when the user clicks the
 * "OK" button in the account editor window.
 *
 * Returns: void.
 **/
G_MODULE_EXPORT
void ggn_edit_window_confirmed (GtkButton *button, gpointer data) {
  /* declare some helping variables. */
  GgnAccount *account;
  const gchar *user;
  const gchar *domain;
  guint idx;

  /* gain a reference to our preferences window. */
  GgnPrefsWindow *window = GGN_PREFS_WINDOW (data);

  /* save the textual contents of the description entry. */
  ggn_account_set_desc (window->priv->editing,
                        gtk_entry_get_text (window->priv->txt_edit_desc));

  /* save the textual contents of the username entry. */
  user = gtk_entry_get_text (window->priv->txt_edit_user);

  /* save the textual contents of the domain entry. */
  domain = gtk_entry_get_text (window->priv->txt_edit_domain);

  /* set the username and domain values. */
  ggn_account_set_email (window->priv->editing, user, domain);

  /* save the textual contents of the password entry. */
  ggn_account_set_pass (window->priv->editing,
                        gtk_entry_get_text (window->priv->txt_edit_pass));

  /* get the boolean value of the widget. */
  if (gtk_toggle_button_get_active (window->priv->chk_edit_def)) {
    /* loop through the list of accounts. */
    for (idx = 0; idx < ggn_account_list_length (window->priv->alist); idx++) {
      /* get the account at this index. */
      account = ggn_account_list_get (window->priv->alist, idx);

      /* set the account to not-default. */
      ggn_account_set_default (account, FALSE);
    }

    /* set this account to default. */
    ggn_account_set_default (window->priv->editing, TRUE);
  }

  /* update the list view of accounts. */
  ggn_prefs_window_reload_listview (window);

  /* hide the editor window. */
  gtk_widget_hide (GTK_WIDGET (window->priv->win_edit));
}

/*
 * ggn_prefs_window_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_prefs_window_init (GgnPrefsWindow *self) {
  /* declare a few helping variables. */
  GtkFileFilter *filter;
  GtkWidget *item;

  /* set up the private data structure. */
  self->priv = g_new0 (GgnPrefsWindowPrivate, 1);

  /* set the currently edited account to null. */
  self->priv->editing = NULL;

  /* allocate a gtkbuilder object. */
  self->priv->xml_prefs = gtk_builder_new ();

  /* load the preferences window xml file. */
  gtk_builder_add_from_file (self->priv->xml_prefs, GGN_XMLUI_PREFS, NULL);

  /* retrieve the preferences window widget. */
  self->priv->win_prefs = GTK_WINDOW (gtk_builder_get_object
                                        (self->priv->xml_prefs,
                                         "GgnPrefsWindow"));

  /* set up the accounts list view. */
  self->priv->list = gtk_list_store_new (LAST_ELEMENT,
                                         G_TYPE_BOOLEAN,
                                         G_TYPE_STRING,
                                         G_TYPE_STRING);

  /* get a reference to the list view. */
  item = GTK_WIDGET (gtk_builder_get_object (self->priv->xml_prefs,
                                             "treeAccounts"));

  /* set the model of the list view. */
  gtk_tree_view_set_model (GTK_TREE_VIEW (item),
                           GTK_TREE_MODEL (self->priv->list));

  /* set up the accounts list view cell renderers. */
  self->priv->cells[ACCOUNT_ENAB] = gtk_cell_renderer_toggle_new ();
  self->priv->cells[ACCOUNT_DESC] = gtk_cell_renderer_text_new ();

  /* set the activatable property of the first cell renderer. */
  g_object_set (G_OBJECT (self->priv->cells[ACCOUNT_ENAB]),
                "activatable", TRUE, NULL);

  /* link the toggled callback to the first cell renderer. */
  g_signal_connect (G_OBJECT (self->priv->cells[ACCOUNT_ENAB]),
                    "toggled",
                    G_CALLBACK (ggn_prefs_window_account_toggled),
                    self);

  /* set up the account enabled column. */
  self->priv->columns[ACCOUNT_ENAB] =
    gtk_tree_view_column_new_with_attributes (_("Enabled"),
                                              self->priv->cells[ACCOUNT_ENAB],
                                              "active", ACCOUNT_ENAB, NULL);

  /* set up the account description column. */
  self->priv->columns[ACCOUNT_DESC] =
    gtk_tree_view_column_new_with_attributes (_("Name"),
                                              self->priv->cells[ACCOUNT_DESC],
                                              "text", ACCOUNT_DESC, NULL);

  /* append the columns to the accounts list view. */
  gtk_tree_view_append_column (GTK_TREE_VIEW (item),
                               self->priv->columns[ACCOUNT_ENAB]);
  gtk_tree_view_append_column (GTK_TREE_VIEW (item),
                               self->priv->columns[ACCOUNT_DESC]);

  /* get the selection from the accounts list view. */
  self->priv->sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (item));

  /* link the changed callback to the list view selection. */
  g_signal_connect (G_OBJECT (self->priv->sel),
                    "changed",
                    G_CALLBACK (ggn_prefs_window_account_selected),
                    self);

  /* get a reference to the file chooser button. */
  self->priv->chooser = GTK_WIDGET (gtk_builder_get_object
                                      (self->priv->xml_prefs,
                                       "btnSoundsChoose"));

  /* create a file-type filter for our chooser. */
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Wave audio files"));
  gtk_file_filter_add_pattern (filter, "*.wav");
  gtk_file_filter_add_mime_type (filter, "audio/x-wav");

  /* apply the new filter to the file chooser. */
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (self->priv->chooser), filter);

  /* connect the signals used by the preferences window and widgets. */
  gtk_builder_connect_signals (self->priv->xml_prefs, self);

  /* allocate another gtkbuilder object. */
  self->priv->xml_edit = gtk_builder_new ();

  /* load the account editor window xml file. */
  gtk_builder_add_from_file (self->priv->xml_edit, GGN_XMLUI_EDIT, NULL);

  /* retrieve the account editor window widget. */
  self->priv->win_edit = GTK_WINDOW (gtk_builder_get_object
                                       (self->priv->xml_edit,
                                        "GgnEditWindow"));

  /* make the account editor window a child of the preferences window. */
  gtk_window_set_modal (self->priv->win_edit, TRUE);
  gtk_window_set_transient_for (self->priv->win_edit, self->priv->win_prefs);

  /* get the description entry widget. */
  self->priv->txt_edit_desc = GTK_ENTRY (gtk_builder_get_object
                                           (self->priv->xml_edit,
                                            "txtName"));

  /* get the username entry widget. */
  self->priv->txt_edit_user = GTK_ENTRY (gtk_builder_get_object
                                           (self->priv->xml_edit,
                                            "txtUser"));

  /* get the domain name entry widget. */
  self->priv->txt_edit_domain = GTK_ENTRY (gtk_builder_get_object
                                             (self->priv->xml_edit,
                                              "txtDomain"));

  /* get the password entry widget. */
  self->priv->txt_edit_pass = GTK_ENTRY (gtk_builder_get_object
                                           (self->priv->xml_edit,
                                            "txtPass"));

  /* get the default value check button widget. */
  self->priv->chk_edit_def = GTK_TOGGLE_BUTTON (gtk_builder_get_object
                                                  (self->priv->xml_edit,
                                                   "chkDefault"));

  /* connect the signals used by the account editor window and widgets. */
  gtk_builder_connect_signals (self->priv->xml_edit, self);
}

/*
 * ggn_prefs_window_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_prefs_window_class_init (GgnPrefsWindowClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_prefs_window_finalize;
}

/*
 * ggn_prefs_window_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_prefs_window_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnPrefsWindow *self = GGN_PREFS_WINDOW (obj);

  /* free the gtkbuilder xml and window widgets. */
  g_object_unref (G_OBJECT (self->priv->xml_edit));
  g_object_unref (G_OBJECT (self->priv->xml_prefs));
  gtk_widget_destroy (GTK_WIDGET (self->priv->win_edit));
  gtk_widget_destroy (GTK_WIDGET (self->priv->win_prefs));

  /* dereference our referenced objects. they will be freed elsewhere. */
  self->priv->alist = NULL;
  self->priv->prefs = NULL;
  self->priv->audio = NULL;

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_prefs_window_parent_class)->finalize (obj);
}

/**
 * ggn_prefs_window_new:
 *
 * Creates a new #GgnPrefsWindow with default values, which are
 * used to edit preferences concerning application functionality.
 *
 * Returns: the new #GgnPrefsWindow object. Free with ggn_prefs_window_free().
 **/
GgnPrefsWindow *ggn_prefs_window_new (void) {
  /* make a newly created gobject. */
  GgnPrefsWindow *window = g_object_new (GGN_TYPE_PREFS_WINDOW, NULL);

  /* return the new object. */
  return window;
}

/**
 * ggn_prefs_window_new_from_manager:
 *
 * Creates a new #GgnPrefsWindow with default values, which are
 * used to edit preferences concerning application functionality.
 * This function, however, adds specific information needed for
 * right functioning.
 *
 * Returns: the new #GgnPrefsWindow object. Free with ggn_prefs_window_free().
 **/
GgnPrefsWindow *ggn_prefs_window_new_from_manager (GgnManager *manager) {
  /* make a newly created gobject. */
  GgnPrefsWindow *window = g_object_new (GGN_TYPE_PREFS_WINDOW, NULL);

  /* pull in references to the manager objects we require. */
  window->priv->alist = manager->priv->alist;
  window->priv->prefs = manager->priv->prefs;
  window->priv->audio = manager->priv->audio;

  /* return the new object. */
  return window;
}

/**
 * ggn_prefs_window_free:
 * @window: The #GgnPrefsWindow to free.
 *
 * Frees the given #GgnPrefsWindow object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_prefs_window_free (GgnPrefsWindow *window) {
  /* while this object exists... */
  while (G_IS_OBJECT (window)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (window));
  }
}

/**
 * ggn_prefs_window_show:
 * @window: the #GgnPrefsWindow to show.
 *
 * Makes the provided preferences window visible by showing its associated
 * #GtkWindow widget.
 *
 * Returns: void.
 **/
void ggn_prefs_window_show (GgnPrefsWindow *window) {
  /* declare some helping variables. */
  GtkWidget *item;
  gchar *soundfile;

  /* if we have a prefs object, set up the widgets with values. */
  if (window->priv->prefs) {
    /* reload the information into the listview. */
    ggn_prefs_window_reload_listview (window);

    /* get a reference to the check rate slider. */
    item = GTK_WIDGET (gtk_builder_get_object (window->priv->xml_prefs,
                                               "slideUpdates"));

    /* set the slider value. */
    gtk_range_set_value (GTK_RANGE (item), (gdouble)
                         ggn_prefs_get_int (window->priv->prefs,
                                            GGN_PREF_CHECKRATE));

    /* get a reference to the message notification check button. */
    item = GTK_WIDGET (gtk_builder_get_object (window->priv->xml_prefs,
                                               "chkNotesMsgs"));

    /* set the check button value. */
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (item),
                                  ggn_prefs_get_bool (window->priv->prefs,
                                                      GGN_PREF_NOTIFY_NEW));

    /* get a reference to the error notification check button. */
    item = GTK_WIDGET (gtk_builder_get_object (window->priv->xml_prefs,
                                               "chkNotesErrs"));

    /* set the check button value. */
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (item),
                                  ggn_prefs_get_bool (window->priv->prefs,
                                                      GGN_PREF_NOTIFY_ERR));

    /* get a reference to the sound playback check button. */
    item = GTK_WIDGET (gtk_builder_get_object (window->priv->xml_prefs,
                                               "chkSounds"));

    /* set the check button value. */
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (item),
                                  ggn_prefs_get_bool (window->priv->prefs,
                                                      GGN_PREF_SOUND_ENAB));

    /* get the sound filename. */
    soundfile = ggn_prefs_get_string (window->priv->prefs,
                                      GGN_PREF_SOUND_FILE);

    /* set the file chooser filename. */
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (window->priv->chooser),
                                   soundfile);

    /* free the sound filename string. */
    g_free (soundfile);
  }

  /* show the preferences window widget. */
  gtk_widget_show (GTK_WIDGET (window->priv->win_prefs));
}

/**
 * ggn_prefs_window_hide:
 * @window: the #GgnPrefsWindow to hide.
 *
 * Makes the provided preferences window invisible by hiding its associated
 * #GtkWindow widget.
 *
 * Returns: void.
 **/
void ggn_prefs_window_hide (GgnPrefsWindow *window) {
  /* get the current sound filename. */
  gchar *filename = gtk_file_chooser_get_filename
                      (GTK_FILE_CHOOSER (window->priv->chooser));

  /* save the current soundfile setting. */
  ggn_prefs_set_string (window->priv->prefs, GGN_PREF_SOUND_FILE, filename);

  /* free the filename string. */
  g_free (filename);

  /* hide the preferences window widget. */
  gtk_widget_hide (GTK_WIDGET (window->priv->win_prefs));
}

