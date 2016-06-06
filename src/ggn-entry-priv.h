/**
 * @file src/ggn-entry-priv.h The GgnEntry object
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
 * GgnEntryPrivate:
 * @summary: The message summary, a snippet of the full text.
 * @title: The message title from the ATOM feed.
 * @sender_email: Email address of the sender.
 * @sender_name: Human-readable name of the sender.
 * @link: The URL to the given message.
 * @id: The identifier that google provides for each message.
 * @state: The state of the entry: new/unread.
 *
 * The #GgnEntry is a simple GObject wrapper for containing GMail
 * email inbox entries that have been parsed from the ATOM feed.
 * Using such a wrapper allows for a more flexible approach to
 * the #GgnAccount objects.
 **/
struct _GgnEntryPrivate {
  /* the email account entry properties. */
  gchar *summary;
  gchar *title;
  gchar *sender_email;
  gchar *sender_name;
  gchar *link;
  gchar *id;

  /* the entry state. */
  guint state;
};

