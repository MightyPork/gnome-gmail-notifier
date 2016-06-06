/**
 * @file src/ggn-account.h The GgnAccount object
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
#ifndef __GNOME_GMAIL_NOTIFIER_ACCOUNT_H__
#define __GNOME_GMAIL_NOTIFIER_ACCOUNT_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * the xpath values that we are to recognize from the parser.
 */
#define GGN_ACCOUNT_XPATH_ENTRY               "/feed/entry"
#define GGN_ACCOUNT_XPATH_ENTRY_SUMMARY       "/feed/entry/summary/text"
#define GGN_ACCOUNT_XPATH_ENTRY_TITLE         "/feed/entry/title/text"
#define GGN_ACCOUNT_XPATH_ENTRY_SENDER_EMAIL  "/feed/entry/author/email/text"
#define GGN_ACCOUNT_XPATH_ENTRY_SENDER_NAME   "/feed/entry/author/name/text"
#define GGN_ACCOUNT_XPATH_ENTRY_LINK          "/feed/entry/link"
#define GGN_ACCOUNT_XPATH_ENTRY_ID            "/feed/entry/id/text"

/*
 * the account launch modes.
 */
#define GGN_ACCOUNT_LAUNCH_INBOX    0
#define GGN_ACCOUNT_LAUNCH_COMPOSE  1

/*
 * the atom feed http request values.
 */
#define GGN_ACCOUNT_FEED_METHOD  "GET"
#define GGN_ACCOUNT_FEED_URI     "https://mail.google.com/mail/feed/atom"

/*
 * the maximum number of attempts before failing to authenticate.
 */
#define GGN_ACCOUNT_MAX_AUTH_ATTEMPTS    3

/*
 * the live account state.
 */
#define GGN_ACCOUNT_STATE_NONE      0
#define GGN_ACCOUNT_STATE_CHECKING  1
#define GGN_ACCOUNT_STATE_CHECKED   2
#define GGN_ACCOUNT_STATE_FAILED    3

/*
 * object function macros.
 */
#define GGN_TYPE_ACCOUNT (ggn_account_get_type ())
#define GGN_ACCOUNT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_ACCOUNT, GgnAccount))
#define GGN_ACCOUNT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_ACCOUNT, GgnAccountClass))
#define GGN_IS_ACCOUNT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_ACCOUNT))
#define GGN_IS_ACCOUNT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_ACCOUNT))
#define GGN_ACCOUNT_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_ACCOUNT, GgnAccountClass))

/*
 * type macros.
 */
typedef struct _GgnAccount GgnAccount;
typedef struct _GgnAccountClass GgnAccountClass;
typedef struct _GgnAccountPrivate GgnAccountPrivate;

/*
 * object definition.
 */
struct _GgnAccount {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnAccountPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnAccountClass {
  /* parent gobject class. */
  GObjectClass parent;

  /* status icon and menu callbacks. */
  void (*modified) (GgnAccount *account);
  void (*updated) (GgnAccount *account);
};

/*
 * gobject-type definition.
 */
GType ggn_account_get_type (void);

/*
 * object creation functions.
 */
GgnAccount *ggn_account_new (void);
GgnAccount *ggn_account_new_from_prefs (GgnPrefs *prefs, const gchar *email);
void ggn_account_free (GgnAccount *account);

/*
 * account string get/set functions.
 */
gchar *ggn_account_get_desc (GgnAccount *account);
void ggn_account_set_desc (GgnAccount *account, const gchar *desc);
gchar *ggn_account_get_user (GgnAccount *account);
void ggn_account_set_user (GgnAccount *account, const gchar *user);
gchar *ggn_account_get_domain (GgnAccount *account);
void ggn_account_set_domain (GgnAccount *account, const gchar *domain);
gchar *ggn_account_get_pass (GgnAccount *account);
void ggn_account_set_pass (GgnAccount *account, const gchar *pass);

/*
 * account email string get/set functions.
 */
gchar *ggn_account_get_email (GgnAccount *account);
void ggn_account_set_email (GgnAccount *account,
                            const gchar *user,
                            const gchar *domain);

/*
 * account boolean get/set functions.
 */
gboolean ggn_account_get_enabled (GgnAccount *account);
void ggn_account_set_enabled (GgnAccount *account, gboolean enab);
gboolean ggn_account_get_default (GgnAccount *account);
void ggn_account_set_default (GgnAccount *account, gboolean def);

/*
 * account mail-checking and launch actions.
 */
void ggn_account_check (GgnAccount *account);
void ggn_account_launch_inbox (GgnAccount *account);
void ggn_account_launch_compose (GgnAccount *account, const gchar *recipient);

/*
 * account reference-giving function.
 */
void ggn_account_give_prefs (GgnAccount *account, GgnPrefs *prefs);

/*
 * account state function.
 */
guint ggn_account_get_state (GgnAccount *account);

/*
 * account entry retrieval functions.
 */
void ggn_account_get_num_entries (GgnAccount *account,
                                  guint *n_unread,
                                  guint *n_new);
GgnEntry *ggn_account_get_entry (GgnAccount *account, guint idx);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_ACCOUNT_H__ */

