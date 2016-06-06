/**
 * @file src/ggn-prefs-window-priv.h The GgnPrefsWindow object
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
 * the data columns for the list view.
 */
enum {
  ACCOUNT_ENAB,
  ACCOUNT_DESC,
  ACCOUNT_EMAIL,
  LAST_ELEMENT
};

/**
 * GgnPrefsWindowPrivate:
 * @xml_prefs: The #GtkBuilder xml gui object for the preferences window.
 * @win_prefs: The #GtkWindow widget for the preferences window.
 * @xml_edit: The #GtkBuilder xml gui object for the account editor window.
 * @win_edit: The #GtkWindow widget for the account editor window.
 * @list: The #GtkTreeModel we are using to list the accounts.
 * @iter: The list iterator used for selections and deletions in the list.
 * @cells: The cells in the list view.
 * @columns: The columns in the list view.
 * @sel: The current list view selection.
 * @chooser: The sound file chooser widget.
 * @alist: The #GgnAccountList we modify to suit the user's needs.
 * @prefs: The #GgnPrefs object we use to manipulate settings.
 * @audio: The #GgnAudio object we use to sample sound files.
 *
 * The #GgnPrefsWindow is a graphical window which takes advantage of the
 * GtkBuilder XML user-interface object to allow the user to interact easily
 * with the gmail notifier for setting preferences and account information.
 **/
struct _GgnPrefsWindowPrivate {
  /* the preferences window. */
  GtkBuilder *xml_prefs;
  GtkWindow *win_prefs;

  /* the account edit window. */
  GtkBuilder *xml_edit;
  GtkWindow *win_edit;

  /* the list-view objects in use. */
  GtkListStore *list;
  GtkTreeIter iter;
  GtkCellRenderer *cells[LAST_ELEMENT];
  GtkTreeViewColumn *columns[LAST_ELEMENT];
  GtkTreeSelection *sel;

  /* the soundfile chooser. */
  GtkWidget *chooser;

  /* the account editor widgets. */
  GtkEntry *txt_edit_desc;
  GtkEntry *txt_edit_user;
  GtkEntry *txt_edit_domain;
  GtkEntry *txt_edit_pass;
  GtkToggleButton *chk_edit_def;

  /* the account we are currently editing, if any. */
  GgnAccount *editing;

  /* the accounts listing for the app. */
  GgnAccountList *alist;

  /* the preferences object for the app. */
  GgnPrefs *prefs;

  /* The audio playback object. */
  GgnAudio *audio;
};

