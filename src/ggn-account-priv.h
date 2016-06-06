/**
 * @file src/ggn-account-priv.h The GgnAccount object
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
 * GgnAccountPrivate:
 * @desc: The descriptive name, unique for all accounts.
 * @user: The account username, all text before the '@'.
 * @domain: The account domain name, such as gmail.com or other for hosted.
 * @pass: The account password, plaintext.
 * @enab: Whether checking this account is enabled.
 * @def: Whether this is the default account or not.
 * @prefs: The #GgnPrefs object to sync to the account.
 * @entries: A #GPtrArray of #GgnEntry objects.
 *
 * The #GgnAccount is a GObject that contains information related to the
 * authentication and inbox status of a single gmail account. All info
 * related to messages (entries) and logins/urls is pulled from a
 * #GgnAccount object.
 **/
struct _GgnAccountPrivate {
  /* the (unique) descriptive name. */
  gchar *desc;

  /* the authentication variables. */
  guint attempts;
  gchar *user;
  gchar *domain;
  gchar *pass;

  /* the status of the account. */
  gboolean enab;
  gboolean def;

  /* reference to our preferences object. */
  GgnPrefs *prefs;

  /* our libsoup objects for atom feed retrieval */
  SoupSession *session;
  SoupMessage *msg;
  SoupMessageBody *msgbody;
  SoupURI *proxy;

  /* our xml parser for the atom feed. */
  GgnXML *xml;

  /* the messages (entries) the account inbox holds. */
  GPtrArray *entries;
  GgnEntry *entry;

  /* the count values for each type of entry. */
  guint num_unread;
  guint num_new;

  /* the account state. */
  guint state;

  /* the callback handler ids, used by the account list. */
  gulong handler_modified;
  gulong handler_updated;
};

/*
 * definitions of private functions used by the GgnAccountList.
 */
void ggn_account_unset_prefs (GgnAccount *account);
void ggn_account_set_prefs (GgnAccount *account);

