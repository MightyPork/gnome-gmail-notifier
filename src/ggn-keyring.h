/**
 * @file src/ggn-keyring.h The GgnKeyring functions
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
 */

/*
 * ensure this is a once-only header.
 */
#ifndef __GNOME_GMAIL_NOTIFIER_KEYRING_H__
#define __GNOME_GMAIL_NOTIFIER_KEYRING_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * gnome-keyring interaction functions.
 */
gboolean ggn_keyring_item_create (const gchar *user,
                                  const gchar *domain,
                                  const gchar *pass);
gboolean ggn_keyring_item_delete (const gchar *user,
                                  const gchar *domain);
gchar *ggn_keyring_item_find (const gchar *user,
                              const gchar *domain);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_KEYRING_H__ */

