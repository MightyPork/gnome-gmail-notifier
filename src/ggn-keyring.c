/**
 * @file src/ggn-keyring.c The GgnKeyring functions
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
 * SECTION:GgnKeyring
 * @short_description: The gnome keyring functions
 * @include: src/ggn-keyring.h
 **/

/*
 * include our application header.
 */
#include <main.h>

/*
 * constants used for gnome-keyring lookups.
 */
#define GGN_KEYRING_USER_NAME        "user"
#define GGN_KEYRING_SERVER_NAME      "server"
#define GGN_KEYRING_SERVER_VAL       "mail.google.com"
#define GGN_KEYRING_OBJECT_NAME      "object"
#define GGN_KEYRING_OBJECT_VAL       "mail/feed/atom"
#define GGN_KEYRING_PROTOCOL_NAME    "protocol"
#define GGN_KEYRING_PROTOCOL_VAL     "https"

/**
 * ggn_keyring_attribs:
 * @user: The username string.
 * @domain: The domain name string.
 *
 * This function will build a G#nomeKeyringAttributeList from the
 * passed username and domain name strings, simplifying access to
 * gnome-keyring.
 *
 * Returns: An allocated #GnomeKeyringAttributeList. Free with
 *          gnome_keyring_attribute_list_free().
 **/
GnomeKeyringAttributeList *ggn_keyring_attribs (const gchar *user,
                                                const gchar *domain) {
  /* initialize an attribute list. */
  GnomeKeyringAttribute attrib;
  GnomeKeyringAttributeList *keyattribs;
  keyattribs = gnome_keyring_attribute_list_new ();

  /* set the "user" attribute. */
  attrib.name = g_strdup (GGN_KEYRING_USER_NAME);
  attrib.type = GNOME_KEYRING_ATTRIBUTE_TYPE_STRING;
  attrib.value.string = g_strdup_printf ("%s@%s", user, domain);
  g_array_append_val (keyattribs, attrib);

  /* set the "server" attribute. */
  attrib.name = g_strdup (GGN_KEYRING_SERVER_NAME);
  attrib.type = GNOME_KEYRING_ATTRIBUTE_TYPE_STRING;
  attrib.value.string = g_strdup (GGN_KEYRING_SERVER_VAL);
  g_array_append_val (keyattribs, attrib);

  /* set the "object" attribute. */
  attrib.name = g_strdup (GGN_KEYRING_OBJECT_NAME);
  attrib.type = GNOME_KEYRING_ATTRIBUTE_TYPE_STRING;
  attrib.value.string = g_strdup (GGN_KEYRING_OBJECT_VAL);
  g_array_append_val (keyattribs, attrib);

  /* set the "protocol" attribute. */
  attrib.name = g_strdup (GGN_KEYRING_PROTOCOL_NAME);
  attrib.type = GNOME_KEYRING_ATTRIBUTE_TYPE_STRING;
  attrib.value.string = g_strdup (GGN_KEYRING_PROTOCOL_VAL);
  g_array_append_val (keyattribs, attrib);

  /* return the attribute list. */
  return keyattribs;
}

/**
 * ggn_keyring_item_create:
 * @user: The username string.
 * @domain: The domain name string.
 * @pass: The passphrase string.
 *
 * This function will access the gnome-keyring daemon to save
 * (or update, if one exists) the given gmail account information
 * into the keyring for use later.
 *
 * Returns: Success #gboolean.
 **/
gboolean ggn_keyring_item_create (const gchar *user,
                                  const gchar *domain,
                                  const gchar *pass) {
  /* declare some helping variables. */
  guint32 id;
  gchar *description;
  GnomeKeyringResult result;
  GnomeKeyringAttributeList *keyattribs;

  /* build the string that describes the keyring entry. */
  description = g_strdup_printf ("Gmail password for %s@%s", user, domain);

  /* define some variables for saving the passphrase. */
  keyattribs = ggn_keyring_attribs (user, domain);

  /* synchronously write to the keyring. */
  result = gnome_keyring_item_create_sync (NULL,
                                           GNOME_KEYRING_ITEM_NETWORK_PASSWORD,
                                           description,
                                           keyattribs,
                                           pass,
                                           TRUE,
                                           &id);

  /* free the attributes list and string. */
  gnome_keyring_attribute_list_free (keyattribs);
  g_free (description);

  /* return our status boolean. */
  if (result == GNOME_KEYRING_RESULT_OK)
    return TRUE;

  /* otherwise, we failed. */
  return FALSE;
}

/**
 * ggn_keyring_item_delete:
 * @user: The username string.
 * @domain: The domain name string.
 *
 * This function will access the gnome-keyring daemon to delete
 * the given gmail account information from the keyring.
 *
 * Returns: Success #gboolean.
 **/
gboolean ggn_keyring_item_delete (const gchar *user,
                                  const gchar *domain) {
  /* declare some helping variables. */
  GList *found;
  GnomeKeyringFound *item;
  GnomeKeyringResult result;
  GnomeKeyringAttributeList *keyattribs;

  /* define some variables for finding the passphrase. */
  keyattribs = ggn_keyring_attribs (user, domain);

  /* search for the password. */
  result = gnome_keyring_find_items_sync (GNOME_KEYRING_ITEM_NETWORK_PASSWORD,
                                          keyattribs,
                                          &found);

  /* did we find a passphrase? */
  if (result == GNOME_KEYRING_RESULT_OK) {
    /* NOTE: read the passphrase from the _first_ found item. */
    item = g_list_nth_data (found, 0);

    /* delete this item. */
    gnome_keyring_item_delete_sync (NULL, item->item_id);
  }

  /* free the variables used for the search. */
  gnome_keyring_found_list_free (found);
  gnome_keyring_attribute_list_free (keyattribs);

  /* return our status boolean. */
  if (result == GNOME_KEYRING_RESULT_OK)
    return TRUE;

  /* otherwise, we failed. */
  return FALSE;
}

/**
 * ggn_keyring_item_find:
 * @user: The username string.
 * @domain: The domain name string.
 *
 * This function will access the gnome-keyring daemon to find
 * the passphrase for the given gmail account information in
 * the keyring.
 *
 * Returns: Passphrase or NULL if not found. Free with g_free().
 **/
gchar *ggn_keyring_item_find (const gchar *user,
                              const gchar *domain) {
  /* declare some helping variables. */
  GList *found;
  gchar *passphrase;
  GnomeKeyringFound *item;
  GnomeKeyringResult result;
  GnomeKeyringAttributeList *keyattribs;

  /* ensure the passphrase is NULL. */
  passphrase = NULL;

  /* define some variables for finding the passphrase. */
  keyattribs = ggn_keyring_attribs (user, domain);

  /* search for the password. */
  result = gnome_keyring_find_items_sync (GNOME_KEYRING_ITEM_NETWORK_PASSWORD,
                                          keyattribs,
                                          &found);

  /* did we find a passphrase? */
  if (result == GNOME_KEYRING_RESULT_OK) {
    /* NOTE: read the passphrase from the _first_ found item. */
    item = g_list_nth_data (found, 0);
    passphrase = g_strdup (item->secret);
  }

  /* free the variables used for the search. */
  gnome_keyring_found_list_free (found);
  gnome_keyring_attribute_list_free (keyattribs);

  /* return the found passphrase or NULL. */
  return passphrase;
}

