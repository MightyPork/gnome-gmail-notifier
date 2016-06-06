/**
 * @file src/ggn-account-list.h The GgnAccountList object
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
#ifndef __GNOME_GMAIL_NOTIFIER_ACCOUNT_LIST_H__
#define __GNOME_GMAIL_NOTIFIER_ACCOUNT_LIST_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * object function macros.
 */
#define GGN_TYPE_ACCOUNT_LIST (ggn_account_list_get_type ())
#define GGN_ACCOUNT_LIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_ACCOUNT_LIST, GgnAccountList))
#define GGN_ACCOUNT_LIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_ACCOUNT_LIST, GgnAccountListClass))
#define GGN_IS_ACCOUNT_LIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_ACCOUNT_LIST))
#define GGN_IS_ACCOUNT_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_ACCOUNT_LIST))
#define GGN_ACCOUNT_LIST_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_ACCOUNT_LIST, GgnAccountListClass))

/*
 * type macros.
 */
typedef struct _GgnAccountList GgnAccountList;
typedef struct _GgnAccountListClass GgnAccountListClass;
typedef struct _GgnAccountListPrivate GgnAccountListPrivate;

/*
 * object definition.
 */
struct _GgnAccountList {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnAccountListPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnAccountListClass {
  /* parent gobject class. */
  GObjectClass parent;

  /* status icon and menu callbacks. */
  void (*modified) (GgnAccountList *alist);
  void (*checked) (GgnAccountList *alist);
  void (*failed) (GgnAccountList *alist);
};

/*
 * gobject-type definition.
 */
GType ggn_account_list_get_type (void);

/*
 * object creation functions.
 */
GgnAccountList *ggn_account_list_new (void);
GgnAccountList *ggn_account_list_new_from_prefs (GgnPrefs *prefs);
void ggn_account_list_free (GgnAccountList *alist);

/*
 * account list management functions.
 */
guint ggn_account_list_length (GgnAccountList *alist);
guint ggn_account_list_get_num_nonempty (GgnAccountList *alist);
guint ggn_account_list_get_num_enabled (GgnAccountList *alist);
guint ggn_account_list_get_num_failed (GgnAccountList *alist);
void ggn_account_list_get_num_entries (GgnAccountList *alist,
                                       guint *n_unread,
                                       guint *n_new);
gboolean ggn_account_list_is_checking (GgnAccountList *alist);
gint ggn_account_list_search (GgnAccountList *alist, const gchar *email);
void ggn_account_list_add (GgnAccountList *alist, GgnAccount *account);
void ggn_account_list_del (GgnAccountList *alist, guint idx);
void ggn_account_list_check (GgnAccountList *alist);

/*
 * account list account retrieval functions.
 */
GgnAccount *ggn_account_list_get (GgnAccountList *alist, guint idx);
GgnAccount *ggn_account_list_get_default (GgnAccountList *alist);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_ACCOUNT_LIST_H__ */

