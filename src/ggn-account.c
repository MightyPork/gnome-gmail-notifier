/**
 * @file src/ggn-account.c The GgnAccount object
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
 * SECTION:GgnAccount
 * @short_description: The gmail account object
 * @include: src/ggn-account.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-account-priv.h"
#include "ggn-entry-priv.h"

/*
 * forward function definitions.
 */
static void ggn_account_init (GgnAccount *self);
static void ggn_account_class_init (GgnAccountClass *klass);
static void ggn_account_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnAccount, ggn_account, G_TYPE_OBJECT);

/*
 * define the signals used.
 */
enum {
  MODIFIED,
  UPDATED,
  LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };

/**
 * ggn_account_default_modified_cb:
 * @account: The account which emitted the signal.
 *
 * This is the default "modified" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_account_default_modified_cb (GgnAccount *account) {
  /* do nothing. */
}

/**
 * ggn_account_default_updated_cb:
 * @account: The account which emitted the signal.
 *
 * This is the default "updated" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_account_default_updated_cb (GgnAccount *account) {
  /* do nothing. */
}

/**
 * ggn_account_soup_auth:
 * @session: The soup session to authenticate.
 * @msg: The message in need of authentication.
 * @auth: The credentials object in use.
 * @retrying: Whether this is a reauthentication.
 * @data: Private data.
 *
 * This function is called when the #SoupSession in question emits the
 * "authenticate" signal, requiring a username and password from the
 * application.
 *
 * Returns: void.
 **/
static void ggn_account_soup_auth (SoupSession *session,
                                   SoupMessage *msg,
                                   SoupAuth *auth,
                                   gboolean retrying,
                                   gpointer data) {
  /* define helping variables. */
  gchar *proxy_user;
  gchar *proxy_pass;
  gchar *email;

  /* gain a reference to our account. */
  GgnAccount *account = GGN_ACCOUNT (data);

  /* avoid infinite loops by failing on retry. */
  if (retrying == TRUE)
    return;

  /* check if its a proxy server asking for authentication. */
  if (soup_auth_is_for_proxy (auth) == TRUE) {
    /* we can only authenticate with a proxy if we have a prefs object!!! */
    if (account->priv->prefs) {
      /* get the proxy username. */
      proxy_user = ggn_prefs_get_string (account->priv->prefs,
                                         GGN_GCONF_PROXY_USER);

      /* get the proxy password. */
      proxy_pass = ggn_prefs_get_string (account->priv->prefs,
                                         GGN_GCONF_PROXY_PASS);

      /* authenticate the program with the proxy. */
      if (proxy_user && proxy_pass)
        soup_auth_authenticate (auth, proxy_user, proxy_pass);

      /* free the proxy credentials strings. */
      g_free (proxy_user);
      g_free (proxy_pass);
    }
  }
  else {
    /* ensure the ATOM feed credentials are non-NULL. */
    if (account->priv->user && account->priv->domain && account->priv->pass) {
      /* determine if this is a hosted account. */
      if (g_utf8_collate (account->priv->domain, "gmail.com") == 0) {
        /* authenticate using the username and password. */
        soup_auth_authenticate (auth, account->priv->user, account->priv->pass);
      }
      else {
        /* build the email string. */
        email = ggn_account_get_email (account);

        /* authenticate using the email and password. */
        soup_auth_authenticate (auth, email, account->priv->pass);

        /* free the email string. */
        g_free (email);
      }
    }
  }
}

/**
 * ggn_account_soup_restart:
 * @msg: The message in need of authentication.
 * @data: Private data.
 *
 * This function is called when the #SoupSession in question emits the
 * "restart" signal, which allows us to count the number of retries
 * that we make at authentication.
 *
 * Returns: void.
 **/
static void ggn_account_soup_restart (SoupMessage *msg, gpointer data) {
  /* gain a reference to our account object. */
  GgnAccount *account = GGN_ACCOUNT (data);

  /* if we've made three attempts, fail. */
  if (account->priv->attempts >= GGN_ACCOUNT_MAX_AUTH_ATTEMPTS) {
    /* abort the message. */
    soup_session_abort (account->priv->session);
  }

  /* otherwise, make another attempt. */
  account->priv->attempts++;
}

/**
 * ggn_account_xml_begin:
 * @xml: The xml parser which emitted the signal.
 * @element_path: The "xpath" of the element parsed.
 * @element_name: The name of the element parsed.
 * @attributes: The attributes of the element.
 *
 * This function is called when the XML parser we are using encounters
 * a newly opened XML tag in the ATOM feed downloaded from gmail.
 *
 * Returns: void.
 **/
static void ggn_account_xml_begin (GgnXML *xml,
                                   gchar *xpath,
                                   gchar *name,
                                   GHashTable *attribs,
                                   gpointer data) {
  /* gain a reference to the account. */
  GgnAccount *account = GGN_ACCOUNT (data);

  /* operate on the xpath. */
  if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY) == 0) {
    /* allocate a new message object. */
    account->priv->entry = ggn_entry_new ();

    /* set the initial message state. */
    account->priv->entry->priv->state = GGN_ENTRY_STATE_NEW;
  }
  else if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY_LINK) == 0) {
    /* read the hyperlink target. */
    account->priv->entry->priv->link = g_strdup (g_hash_table_lookup (attribs,
                                                                      "href"));
  }
}

/**
 * ggn_account_xml_end:
 * @xml: The xml parser which emitted the signal.
 * @element_path: The "xpath" of the element parsed.
 * @element_name: The name of the element parsed.
 *
 * This function is called when the XML parser we are using encounters
 * a closed XML tag in the ATOM feed downloaded from gmail.
 *
 * Returns: void.
 **/
static void ggn_account_xml_end (GgnXML *xml,
                                 gchar *xpath,
                                 gchar *name,
                                 gpointer data) {
  /* declare a few helping variables. */
  GgnEntry *arrentry;
  guint idx;

  /* gain a reference to the account. */
  GgnAccount *account = GGN_ACCOUNT (data);

  /* operate on the xpath. */
  if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY) == 0) {
    /* loop through the entries array. */
    for (idx = 0; idx < account->priv->entries->len; idx++) {
      /* get a reference to the array entry. */
      arrentry = g_ptr_array_index (account->priv->entries, idx);

      /* see if we have a matching entry in the array already. */
      if (g_utf8_collate (arrentry->priv->id,
                          account->priv->entry->priv->id) == 0) {
        /* we have a match, don't add this entry. */
        arrentry->priv->state = GGN_ENTRY_STATE_UNREAD;
        ggn_entry_free (account->priv->entry);
        account->priv->num_unread++;
        return;
      }
    }

    /* no matches, move the current entry into the pointer array. */
    g_ptr_array_add (account->priv->entries, account->priv->entry);
    account->priv->num_new++;
  }
}

/**
 * ggn_account_xml_text:
 * @xml: The xml parser which emitted the signal.
 * @element_path: The "xpath" of the current element.
 * @element_name: The name of the current element.
 * @value: The text we wave parsed from the XML.
 *
 * This function is called when the XML parser we are using encounters
 * textual data in the ATOM feed downloaded from gmail.
 *
 * Returns: void.
 **/
static void ggn_account_xml_text (GgnXML *xml,
                                  gchar *xpath,
                                  gchar *name,
                                  gchar *value,
                                  gpointer data) {
  /* gain a reference to the account. */
  GgnAccount *account = GGN_ACCOUNT (data);

  /* operate on the xpath. */
  if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY_SUMMARY) == 0) {
    /* set the value in the entry private data. */
    account->priv->entry->priv->summary = g_strdup (value);
  }
  else if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY_TITLE) == 0) {
    /* set the value in the entry private data. */
    account->priv->entry->priv->title = g_strdup (value);
  }
  else if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY_SENDER_EMAIL) == 0) {
    /* set the value in the entry private data. */
    account->priv->entry->priv->sender_email = g_strdup (value);
  }
  else if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY_SENDER_NAME) == 0) {
    /* set the value in the entry private data. */
    account->priv->entry->priv->sender_name = g_strdup (value);
  }
  else if (g_utf8_collate (xpath, GGN_ACCOUNT_XPATH_ENTRY_ID) == 0) {
    /* set the value in the entry private data. */
    account->priv->entry->priv->id = g_strdup (value);
  }
}

/**
 * ggn_account_entries_clear:
 * @account: The account to modify.
 *
 * Clears the entries #GPtrArray of a given #GgnAccount. private only.
 *
 * Returns: void.
 **/
void ggn_account_entries_clear (GgnAccount *account) {
  /* define a helping variable. */
  gpointer ptr;

  /* loop until the pointer array is empty. */
  while (account->priv->entries->len > 0) {
    /* remove the first pointer. */
    ptr = g_ptr_array_index (account->priv->entries, 0);
    g_ptr_array_remove_fast (account->priv->entries, ptr);

    /* free the entry (pointer). */
    ggn_entry_free ((GgnEntry *) ptr);
  }
}

/*
 * ggn_account_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_account_init (GgnAccount *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnAccountPrivate, 1);

  /* set up the initial values for the private instance objects. */
  self->priv->attempts = 0;
  self->priv->desc = NULL;
  self->priv->user = NULL;
  self->priv->domain = NULL;
  self->priv->pass = NULL;
  self->priv->enab = FALSE;
  self->priv->def = FALSE;

  /* set up the initial NULL reference to the prefs object. */
  self->priv->prefs = NULL;

  /* initialize the soup session objects. */
  self->priv->session = soup_session_sync_new ();
  self->priv->msg = NULL;
  self->priv->msgbody = NULL;
  self->priv->proxy = NULL;

  /* link the soup session to our auth callback. */
  g_signal_connect (G_OBJECT (self->priv->session),
                    "authenticate",
                    G_CALLBACK (ggn_account_soup_auth),
                    self);

  /* initialize the xml parser. */
  self->priv->xml = ggn_xml_new ();

  /* link the begin-element callback to our callback function. */
  g_signal_connect (G_OBJECT (self->priv->xml),
                    "begin_element",
                    G_CALLBACK (ggn_account_xml_begin),
                    self);

  /* link the end-element callback to our callback function. */
  g_signal_connect (G_OBJECT (self->priv->xml),
                    "end_element",
                    G_CALLBACK (ggn_account_xml_end),
                    self);

  /* link the text callback to our callback function. */
  g_signal_connect (G_OBJECT (self->priv->xml),
                    "text",
                    G_CALLBACK (ggn_account_xml_text),
                    self);

  /* initialize the entries pointer array. */
  self->priv->entries = g_ptr_array_new ();
  self->priv->entry = NULL;

  /* initialize the entry counters. */
  self->priv->num_unread = 0;
  self->priv->num_new = 0;

  /* set up the account state. */
  self->priv->state = GGN_ACCOUNT_STATE_NONE;
}

/*
 * ggn_account_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_account_class_init (GgnAccountClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_account_finalize;

  /* setup the default signal handlers. */
  klass->modified = ggn_account_default_modified_cb;
  klass->updated = ggn_account_default_updated_cb;

  /**
   * GgnAccount::modified:
   * @account: The #GgnAccount object that issued the callback.
   *
   * Emitted when the account information is changed by a get/set function
   **/
  signals[MODIFIED] =
    g_signal_new ("modified",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnAccountClass, modified),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * GgnAccount::updated:
   * @account: The #GgnAccount object that issued the callback.
   *
   * Emitted when the account status is changed by a check action.
   **/
  signals[UPDATED] =
    g_signal_new ("updated",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnAccountClass, updated),
                  NULL, NULL,
                  ggn_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

/*
 * ggn_account_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_account_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnAccount *self = GGN_ACCOUNT (obj);

  /* set the account state. */
  self->priv->state = GGN_ACCOUNT_STATE_NONE;

  /* free the strings. */
  g_free (self->priv->desc);
  g_free (self->priv->user);
  g_free (self->priv->domain);
  g_free (self->priv->pass);

  /* reduce the reference count on our prefs object. */
  if (self->priv->prefs)
    g_object_unref (G_OBJECT (self->priv->prefs));

  /* unreference the soup session object. */
  g_object_unref (G_OBJECT (self->priv->session));

  /* free the soup proxy uri. */
  if (self->priv->proxy)
    soup_uri_free (self->priv->proxy);

  /* free the xml parser. */
  ggn_xml_free (self->priv->xml);

  /* free the entries list. */
  ggn_account_entries_clear (self);
  g_ptr_array_free (self->priv->entries, TRUE);

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_account_parent_class)->finalize (obj);
}

/**
 * ggn_account_new:
 *
 * Creates a new #GgnAccount with default values, which are
 * used to access and manipulate account inbox information
 *
 * Returns: the new #GgnAccount object. Free with ggn_account_free().
 **/
GgnAccount *ggn_account_new (void) {
  /* make a newly created gobject. */
  GgnAccount *account = g_object_new (GGN_TYPE_ACCOUNT, NULL);

  /* return the new object. */
  return account;
}

/**
 * ggn_account_new_from_prefs:
 *
 * Creates a new #GgnAccount with values that are pulled from
 * the preferences structure, using the email datafield.
 *
 * Returns: the new #GgnAccount object. Free with ggn_account_free().
 **/
GgnAccount *ggn_account_new_from_prefs (GgnPrefs *prefs, const gchar *email) {
  /* declare a helping variable. */
  gchar *key;

  /* make a newly created object. */
  GgnAccount *account = g_object_new (GGN_TYPE_ACCOUNT, NULL);

  /* only if we passed a non-null prefs object. */
  if (prefs) {
    /* set the account prefs object. */
    ggn_account_give_prefs (account, prefs);

    /* read in the description string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DESC, email);
    account->priv->desc = ggn_prefs_get_string (account->priv->prefs, key);
    g_free (key);

    /* read in the username string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_USER, email);
    account->priv->user = ggn_prefs_get_string (account->priv->prefs, key);
    g_free (key);

    /* read in the domain name string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DOMAIN, email);
    account->priv->domain = ggn_prefs_get_string (account->priv->prefs, key);
    g_free (key);

    /* read in the enabled boolean. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_ENABLED, email);
    account->priv->enab = ggn_prefs_get_bool (account->priv->prefs, key);
    g_free (key);

    /* read in the default boolean. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DEFAULT, email);
    account->priv->def = ggn_prefs_get_bool (account->priv->prefs, key);
    g_free (key);

    /* read in the password string. */
    account->priv->pass = ggn_keyring_item_find (account->priv->user,
                                                 account->priv->domain);
  }

  /* return the new object. */
  return account;
}

/**
 * ggn_account_free:
 * @account: The #GgnAccount to free.
 *
 * Frees the given #GgnAccount object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_account_free (GgnAccount *account) {
  /* while this object exists... */
  while (G_IS_OBJECT (account)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (account));
  }
}

/**
 * ggn_account_unset_prefs:
 * @account: The #GgnAccount to free.
 *
 * Unsets the preferences and keyring data associated with the given
 * #GgnAccount object, used privately for account deletion and modification.
 *
 * Returns: void.
 **/
void ggn_account_unset_prefs (GgnAccount *account) {
  /* declare a few helping variables. */
  GSList *accounts;
  GSList *found;
  gchar *email;
  gchar *key;

  /* this only works if we have preferences. */
  if (account->priv->prefs && account->priv->user && account->priv->domain) {
    /* build the email string. */
    email = ggn_account_get_email (account);

    /* build the gconf key root string. */
    key = g_strdup_printf ("%s/%s", GGN_PREF_ACCOUNT_ROOT, email);

    /* unset the old gconf account tree. */
    ggn_prefs_unset (account->priv->prefs, key);

    /* unset the old keyring entry. */
    ggn_keyring_item_delete (account->priv->user, account->priv->domain);

    /* retrieve the accounts gconf list. */
    accounts = ggn_prefs_get_list (account->priv->prefs,
                                   GGN_PREF_ACCOUNT_LIST);

    /* operate on the list. */
    if (accounts) {
      /* find the account in question. */
      found = g_slist_find_custom (accounts, email,
                                   (GCompareFunc) g_utf8_collate);

      /* operate on the account. */
      if (found) {
        /* remove the entry from the list. */
        accounts = g_slist_remove_link (accounts, found);
        g_slist_free (found);

        /* return the list to gconf. */
        ggn_prefs_set_list (account->priv->prefs,
                            GGN_PREF_ACCOUNT_LIST,
                            accounts);

        /* free the accounts linked list. */
        g_slist_free (accounts);
      }
    }

    /* free the built strings. */
    g_free (email);
    g_free (key);
  }
}

/**
 * ggn_account_set_prefs:
 * @account: The #GgnAccount to free.
 *
 * Sets the preferences and keyring data associated with the given
 * #GgnAccount object, used privately for account addition and modification.
 *
 * Returns: void.
 **/
void ggn_account_set_prefs (GgnAccount *account) {
  /* declare a few helping variables. */
  GSList *accounts;
  gchar *email;
  gchar *key;

  /* this only works if we have preferences. */
  if (account->priv->prefs && account->priv->user && account->priv->domain) {
    /* build the email string. */
    email = ggn_account_get_email (account);

    /* resave the description string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DESC, email);
    ggn_prefs_set_string (account->priv->prefs, key, account->priv->desc);
    g_free (key);

    /* resave the username string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_USER, email);
    ggn_prefs_set_string (account->priv->prefs, key, account->priv->user);
    g_free (key);

    /* resave the domain name string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DOMAIN, email);
    ggn_prefs_set_string (account->priv->prefs, key, account->priv->domain);
    g_free (key);

    /* resave the enabled boolean. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_ENABLED, email);
    ggn_prefs_set_bool (account->priv->prefs, key, account->priv->enab);
    g_free (key);

    /* resave the default boolean. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DEFAULT, email);
    ggn_prefs_set_bool (account->priv->prefs, key, account->priv->def);
    g_free (key);

    /* create the keyring entry. */
    ggn_keyring_item_create (account->priv->user,
                             account->priv->domain,
                             account->priv->pass);

    /* retrieve the accounts gconf list. */
    accounts = ggn_prefs_get_list (account->priv->prefs,
                                   GGN_PREF_ACCOUNT_LIST);

    /* append the account to the list. */
    accounts = g_slist_append (accounts, email);

    /* return the list to gconf. */
    ggn_prefs_set_list (account->priv->prefs,
                        GGN_PREF_ACCOUNT_LIST,
                        accounts);

    /* free the accounts linked list. */
    g_slist_free (accounts);
  }
}

/**
 * ggn_account_get_desc:
 * @account: The #GgnAccount to access.
 *
 * Request the description string of the #GgnAccount object.
 *
 * Returns: An in-use string. Freed automatically by ggn_account_free().
 **/
gchar *ggn_account_get_desc (GgnAccount *account) {
  /* return the value. */
  return account->priv->desc;
}

/**
 * ggn_account_set_desc:
 * @account: The #GgnAccount to access.
 * @desc: The new value.
 *
 * Sets the description string of the #GgnAccount object.
 *
 * Returns: void.
 **/
void ggn_account_set_desc (GgnAccount *account, const gchar *desc) {
  /* declare a few helping variables. */
  gchar *email;
  gchar *key;

  /* properly free and set the new value. */
  g_free (account->priv->desc);
  account->priv->desc = g_strdup (desc);

  /* update the preferences value. */
  if (account->priv->prefs && account->priv->user && account->priv->domain) {
    /* build the email string. */
    email = ggn_account_get_email (account);

    /* build the preference key string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DESC, email);

    /* set the value in the prefs database. */
    ggn_prefs_set_string (account->priv->prefs, key, desc);

    /* free the key and email strings. */
    g_free (email);
    g_free (key);
  }
}

/**
 * ggn_account_get_user:
 * @account: The #GgnAccount to access.
 *
 * Request the username string of the #GgnAccount object.
 *
 * Returns: An in-use string. Freed automatically by ggn_account_free().
 **/
gchar *ggn_account_get_user (GgnAccount *account) {
  /* return the value. */
  return account->priv->user;
}

/**
 * ggn_account_set_user:
 * @account: The #GgnAccount to access.
 * @user: The new value.
 *
 * Sets the username string of the #GgnAccount object.
 *
 * Returns: void.
 **/
void ggn_account_set_user (GgnAccount *account, const gchar *user) {
  /* unset the preferences for the old username. */
  ggn_account_unset_prefs (account);

  /* properly free and set the new value. */
  g_free (account->priv->user);
  account->priv->user = g_strdup (user);

  /* set the preferences for the new username. */
  ggn_account_set_prefs (account);
}

/**
 * ggn_account_get_domain:
 * @account: The #GgnAccount to access.
 *
 * Request the domain name string of the #GgnAccount object.
 *
 * Returns: An in-use string. Freed automatically by ggn_account_free().
 **/
gchar *ggn_account_get_domain (GgnAccount *account) {
  /* return the value. */
  return account->priv->domain;
}

/**
 * ggn_account_set_domain:
 * @account: The #GgnAccount to access.
 * @domain: The new value.
 *
 * Sets the domain name string of the #GgnAccount object.
 *
 * Returns: void.
 **/
void ggn_account_set_domain (GgnAccount *account, const gchar *domain) {
  /* unset the preferences for the old domain name. */
  ggn_account_unset_prefs (account);

  /* properly free and set the new value. */
  g_free (account->priv->domain);
  account->priv->domain = g_strdup (domain);

  /* set the preferences for the new domain name. */
  ggn_account_set_prefs (account);
}

/**
 * ggn_account_get_pass:
 * @account: The #GgnAccount to access.
 *
 * Request the password string of the #GgnAccount object.
 *
 * Returns: An in-use string. Freed automatically by ggn_account_free().
 **/
gchar *ggn_account_get_pass (GgnAccount *account) {
  /* return the value. */
  return account->priv->pass;
}

/**
 * ggn_account_set_pass:
 * @account: The #GgnAccount to access.
 * @pass: The new value.
 *
 * Sets the password string of the #GgnAccount object.
 *
 * Returns: void.
 **/
void ggn_account_set_pass (GgnAccount *account, const gchar *pass) {
  /* properly free and set the new value. */
  g_free (account->priv->pass);
  account->priv->pass = g_strdup (pass);

  /* update the keyring value. */
  if (account->priv->user && account->priv->domain) {
    /* create (or update if existing) the pass for user at domain. */
    ggn_keyring_item_create (account->priv->user,
                             account->priv->domain,
                             account->priv->pass);
  }
}

/**
 * ggn_account_get_email:
 * @account: The #GgnAccount to access.
 *
 * Request the email string of the #GgnAccount object.
 *
 * Returns: A newly allocated email string. Free with g_free().
 **/
gchar *ggn_account_get_email (GgnAccount *account) {
  /* return the value. */
  return g_strdup_printf ("%s@%s", account->priv->user, account->priv->domain);
}

/**
 * ggn_account_set_email:
 * @account: The #GgnAccount to access.
 * @user: The new username string.
 * @domain: The new domain name string.
 *
 * Simultaneously sets the username and domain name strings in the
 * #GgnAccount object, saving time when the user edits the accounts.
 *
 * Returns: void.
 **/
void ggn_account_set_email (GgnAccount *account,
                            const gchar *user,
                            const gchar *domain) {
  /* unset the preferences for the old email. */
  ggn_account_unset_prefs (account);

  /* properly free and set the new value. */
  g_free (account->priv->user);
  g_free (account->priv->domain);
  account->priv->user = g_strdup (user);
  account->priv->domain = g_strdup (domain);

  /* set the preferences for the new email. */
  ggn_account_set_prefs (account);
}

/**
 * ggn_account_get_enabled:
 * @account: The #GgnAccount to access.
 *
 * Request the enabled state of the #GgnAccount object.
 *
 * Returns: A #gboolean representing the enabled state.
 **/
gboolean ggn_account_get_enabled (GgnAccount *account) {
  /* return the value. */
  return account->priv->enab;
}

/**
 * ggn_account_set_enabled:
 * @account: The #GgnAccount to access.
 * @enab: The new value.
 *
 * Sets the enabled state of the #GgnAccount object.
 *
 * Returns: void.
 **/
void ggn_account_set_enabled (GgnAccount *account, gboolean enab) {
  /* declare a few helping variables. */
  gchar *email;
  gchar *key;

  /* set the new value. */
  account->priv->enab = enab;

  /* ensure we *can* update the prefs value. */
  if (account->priv->prefs && account->priv->user && account->priv->domain) {
    /* build the email string. */
    email = ggn_account_get_email (account);

    /* build the preference key string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_ENABLED, email);

    /* set the value in the prefs database. */
    ggn_prefs_set_bool (account->priv->prefs, key, enab);

    /* free the key and email strings. */
    g_free (email);
    g_free (key);
  }
}

/**
 * ggn_account_get_default:
 * @account: The #GgnAccount to access.
 *
 * Request the defaulted state of the #GgnAccount object.
 *
 * Returns: A #gboolean representing the defaulted state.
 **/
gboolean ggn_account_get_default (GgnAccount *account) {
  /* return the value. */
  return account->priv->def;
}

/**
 * ggn_account_set_default:
 * @account: The #GgnAccount to access.
 * @def: The new value.
 *
 * Sets the defaulted state of the #GgnAccount object.
 *
 * Returns: void.
 **/
void ggn_account_set_default (GgnAccount *account, gboolean def) {
  /* declare a few helping variables. */
  gchar *email;
  gchar *key;

  /* set the new value. */
  account->priv->def = def;

  /* ensure we *can* update the prefs value. */
  if (account->priv->prefs && account->priv->user && account->priv->domain) {
    /* build the email string. */
    email = ggn_account_get_email (account);

    /* build the preference key string. */
    key = g_strdup_printf (GGN_PREF_ACCOUNT_DEFAULT, email);

    /* set the value in the prefs database. */
    ggn_prefs_set_bool (account->priv->prefs, key, def);

    /* free the key and email strings. */
    g_free (email);
    g_free (key);
  }
}

/**
 * ggn_account_check_thread:
 * @account: The #GgnAccount to run the check with.
 *
 * This is the private thread function called by ggn_account_check() in
 * the #GgnAccount code to check the inbox conditions and parse the xml
 * atom feed.
 *
 * Returns: void.
 **/
gpointer ggn_account_check_thread (gpointer data) {
  /* declare a few helping variables. */
  gboolean success;
  guint status;
  gchar *body;
  guint idx;

  /* gain a reference to our account object. */
  GgnAccount *account = GGN_ACCOUNT (data);

  /* set the account state. */
  account->priv->state = GGN_ACCOUNT_STATE_CHECKING;

  /* let's be optimistic. */
  success = TRUE;

  /* create an http request message. */
  account->priv->msg = soup_message_new (GGN_ACCOUNT_FEED_METHOD,
                                         GGN_ACCOUNT_FEED_URI);

  /* link the restart callback to our message. */
  g_signal_connect (G_OBJECT (account->priv->msg),
                    "restarted",
                    G_CALLBACK (ggn_account_soup_restart),
                    account);

  /* reset the number of authentication attempts. */
  account->priv->attempts = 0;

  /* send the message into the etherwebs. */
  status = soup_session_send_message (account->priv->session,
                                      account->priv->msg);

  /* check the return status. */
  if (status == SOUP_STATUS_OK) {
    /* reset the number of counted entries. */
    account->priv->num_unread = 0;
    account->priv->num_new = 0;

    /* extract the message text from the response. */
    account->priv->msgbody = account->priv->msg->response_body;
    soup_message_body_flatten (account->priv->msgbody);
    body = g_strdup (account->priv->msgbody->data);

    /* set all entries in the array to "read". */
    for (idx = 0; idx < account->priv->entries->len; idx++) {
      /* gain a reference to the indexed entry and set the state to "read". */
      account->priv->entry = g_ptr_array_index (account->priv->entries, idx);
      account->priv->entry->priv->state = GGN_ENTRY_STATE_READ;
    }

    /* parse the xml from the downloaded feed. */
    if (!ggn_xml_load_string (account->priv->xml, body)) {
      /* unset our success boolean. */
      success = FALSE;
    }
    else {
      /* clean up entries still marked "read". */
      for (idx = 0; idx < account->priv->entries->len; idx++) {
        /* gain a reference to the indexed entry. */
        account->priv->entry = g_ptr_array_index (account->priv->entries, idx);

        /* see if this entry has been read. */
        if (account->priv->entry->priv->state == GGN_ENTRY_STATE_READ) {
          /* free and delete the entry and fix the counter integer. */
          g_ptr_array_remove_index (account->priv->entries, idx);
          ggn_entry_free (account->priv->entry);
          idx--;
        }
      }
    }

    /* free the message body string. */
    g_free (body);
  }
  else {
    /* unset our success boolean. */
    success = FALSE;
  }

  /* close all open and unused connections. */
  soup_session_abort (account->priv->session);

  /* set the appropriate status. */
  if (success == TRUE) {
    /* we successfully checked the account. */
    account->priv->state = GGN_ACCOUNT_STATE_CHECKED;
  }
  else {
    /* an error occurred somewhere along the way. */
    account->priv->state = GGN_ACCOUNT_STATE_FAILED;
  }

  /* emit the updated signal. */
  g_signal_emit (account, signals[UPDATED], 0);

  /* return from the function. */
  return data;
}

/**
 * ggn_account_check:
 * @account: The #GgnAccount to run the check with.
 *
 * Initiates an asynchronous mail checking operation which returns by
 * emitting one of the signals defined in the #GgnAccount code.
 *
 * Returns: void.
 **/
void ggn_account_check (GgnAccount *account) {
  /* create the new thread. */
  g_thread_create (ggn_account_check_thread, (gpointer) account, FALSE, NULL);
}

/**
 * ggn_account_launch:
 * @account: A #GgnAccount object.
 * @mode: Either GGN_ACCOUNT_LAUNCH_INBOX or GGN_ACCOUNT_LAUNCH_COMPOSE.
 * @user: The username string.
 * @domain: The domain name string. 
 * @pass: The password string.
 * @mailto: Our recipient, or NULL.
 *
 * Launches the web-based gmail of the given gmail account using the function
 * ggn_link_execute(). Private use only.
 *
 * Returns: void.
 **/
void ggn_account_launch (GgnAccount *account, int mode,
                         const gchar *user, const gchar *domain,
                         const gchar *pass, const gchar *mailto) {
  /* declare some variables. */
  gchar *authurl = NULL;
  gchar *navurl = NULL;

  /* declare some constants. */
  const gchar *navmode = ((mode == GGN_ACCOUNT_LAUNCH_COMPOSE) ?
                           "view=cm&fs=1&nsr=0" : "");
  const gchar *authserv = "&service=mail&rm=false&Email=";
  const gchar *authpost = "&rmShown=1&signIn=Sign+in";

  /* determine the launch mode. */
  if (g_utf8_collate (domain, "gmail.com") == 0) {
    /* set the post-auth navigation url. */
    navurl = g_strdup_printf ("%s?%s%s%s", "https://mail.google.com/mail/",
               navmode, mailto ? "&to=" : "", mailto ? mailto : "");

    /* vanilla gmail site. */
    authurl = g_strdup_printf ("%s%s%s%s%s@%s%s%s%s",
                "https://www.google.com/accounts/ServiceLoginAuth",
                "?ltmpl=default&ltmplcache=2&continue=",
                g_uri_escape_string (navurl, "", TRUE),
                authserv, user, domain, "&Passwd=", pass, authpost);
  }
  else {
    /* set the post-auth navigation url. */
    navurl = g_strdup_printf ("%s%s/?%s%s%s", "https://mail.google.com/a/",
               domain, navmode, mailto ? "&to=" : "", mailto ? mailto : "");

    /* hosted gmail site. */
    authurl = g_strdup_printf ("%s%s%s%s%s%s%s%s%s",
                "https://www.google.com/a/", domain,
                "/LoginAction2?ltmpl=default&ltmplcache=2&continue=",
                g_uri_escape_string (navurl, "", TRUE),
                authserv, user, "&Passwd=", pass, authpost);
  }

  /* execute the url we have constructed. */
  ggn_link_execute (authurl);

  /* free the strings used. */
  g_free (authurl);
  g_free (navurl);
}

/**
 * ggn_account_launch_inbox:
 * @account: The #GgnAccount to use.
 *
 * Launches the web-based inbox of the given gmail account using the function
 * ggn_account_launch().
 *
 * Returns: void.
 **/
void ggn_account_launch_inbox (GgnAccount *account) {
  /* utilize ggn_account_launch(). */
  ggn_account_launch (account,
                      GGN_ACCOUNT_LAUNCH_INBOX,
                      account->priv->user,
                      account->priv->domain,
                      account->priv->pass,
                      NULL);
}

/**
 * ggn_account_launch_compose:
 * @account: The #GgnAccount to use.
 * @recipient: The email address to send this email to.
 *
 * Launches the web-based composition page of the given gmail account
 * using the function ggn_account_launch().
 *
 * Returns: void.
 **/
void ggn_account_launch_compose (GgnAccount *account, const gchar *recipient) {
  /* utilize ggn_account_launch(). */
  ggn_account_launch (account,
                      GGN_ACCOUNT_LAUNCH_COMPOSE,
                      account->priv->user,
                      account->priv->domain,
                      account->priv->pass,
                      recipient);
}

/**
 * ggn_account_give_prefs:
 * @account: The #GgnAccount to give the #GgnPrefs to.
 * @prefs: The #GgnPrefs pointer to give to the account.
 *
 * This function gives the given #GgnAccount object a reference to the
 * given #GgnPrefs object and increases the refcount of the prefs object,
 * to allow the account to automatically update its settings.
 *
 * Returns: void.
 **/
void ggn_account_give_prefs (GgnAccount *account, GgnPrefs *prefs) {
  /* declare a few helping variables. */
  gchar *ssl_host;
  gchar *host;
  gchar *uri;
  gint port;
  gint ssl_port;

  /* ensure the prefs object is non-NULL. */
  if (!prefs)
    return;

  /* hand the pointer off. */
  account->priv->prefs = prefs;

  /* increment the reference count. */
  g_object_ref (prefs);

  /* immediately check to see if we should setup the proxy. */
  if (ggn_prefs_get_bool (account->priv->prefs, GGN_GCONF_PROXY_USE)) {
    /* get the insecure proxy hostname. */
    host = ggn_prefs_get_string (account->priv->prefs, GGN_GCONF_PROXY_HOST);

    /* get the insecure proxy port. */
    port = ggn_prefs_get_int (account->priv->prefs, GGN_GCONF_PROXY_PORT);

    /* get the secure proxy hostname. */
    ssl_host = ggn_prefs_get_string (account->priv->prefs,
                                     GGN_GCONF_PROXY_SSL_HOST);

    /* get the secure proxy port. */
    ssl_port = ggn_prefs_get_int (account->priv->prefs,
                                  GGN_GCONF_PROXY_SSL_PORT);

    /* try to use secured values first. */
    if ((g_utf8_strlen (ssl_host, -1) > 0) && (ssl_port != 0)) {
      /* use the secure host and port. */
      uri = g_strdup_printf ("http://%s:%d", ssl_host, ssl_port);
    }
    else if ((g_utf8_strlen (host, -1) > 0) && (port != 0)) {
      /* use the insecure host and port. */
      uri = g_strdup_printf ("http://%s:%d", host, port);
    }
    else {
      /* we could not find the proper arguments. */
      uri = NULL;
    }

    /* create the proxy uri. */
    account->priv->proxy = soup_uri_new (uri);

    /* update the proxy for the session. */
    if (account->priv->proxy) {
      /* set the proxy-uri property for the session. */
      g_object_set (G_OBJECT (account->priv->session),
                    SOUP_SESSION_PROXY_URI,
                    account->priv->proxy,
                    NULL);
    }

    /* free the allocated strings. */
    g_free (ssl_host);
    g_free (host);
    g_free (uri);
  }
}

/**
 * ggn_account_get_state:
 * @account: The #GgnAccount to access.
 *
 * This function returns the live state of the gmail account.
 *
 * Returns: A #guint of the live state.
 **/
guint ggn_account_get_state (GgnAccount *account) {
  /* easy peasy. */
  return account->priv->state;
}

/**
 * ggn_account_get_num_entries:
 * @account: The #GgnAccount to access.
 * @n_unread: A pointer to a #guint to hold the unread entry count.
 * @n_new: A pointer to a #guint to hold the new entry count.
 *
 * This function returns the number of messages (entries) that are
 * newly unread in the inbox of the given #GgnAccount object.
 *
 * Returns: void.
 **/
void ggn_account_get_num_entries (GgnAccount *account,
                                  guint *n_unread,
                                  guint *n_new) {
  /* set the values of the pointed-to variables. */
  *n_unread = account->priv->num_unread;
  *n_new = account->priv->num_new;
}

/**
 * ggn_account_get_entry:
 * @account: The #GgnAccount to access.
 * @idx: The index of the #GgnEntry to retrieve.
 *
 * This function returns a pointer to a given message (entry) in
 * the account object. If the index is out of bounds or an error
 * occurs in returning the entry, NULL is returned.
 *
 * Returns: A pointer to a #GgnEntry object. Automatically freed!
 **/
GgnEntry *ggn_account_get_entry (GgnAccount *account, guint idx) {
  /* check bounds. */
  if (idx >= account->priv->entries->len)
    return NULL;

  /* return the value. */
  return g_ptr_array_index (account->priv->entries, idx);
}

