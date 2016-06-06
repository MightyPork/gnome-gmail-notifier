/**
 * @file src/ggn-account-list-priv.h The GgnAccountList object
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
 * GgnAccountListPrivate:
 * @prefs: The #GgnPrefs object we use to track changes.
 * @accounts: A #GPtrArray full of #GgnAccount objects.
 * @account: A #GgnAccount storage element for operations.
 * @state: The checking/check/fail state of the accounts in the list.
 *
 * The #GgnAccountList is a GObject that manages the creation, modification,
 * updating, and deletion of gmail accounts.
 **/
struct _GgnAccountListPrivate {
  /* the preferences object we use. */
  GgnPrefs *prefs;

  /* the accounts that the account list holds. */
  GPtrArray *accounts;
  GgnAccount *account;

  /* the count variables for accounts the list holds. */
  guint num_nonempty;
  guint num_failed;

  /* the count variables for the held entries. */
  guint num_unread_entries;
  guint num_new_entries;
};

