/**
 * @file src/ggn-xml-priv.h The GgnXML object
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
 * GgnXMLPrivate:
 * @reader: The Libxml2 XML Parsing Struct.
 * @type: The node type of the current XML node.
 * @name: The name of the current XML node.
 * @text: The text contained in the current XML node.
 * @xpath: A non-conforming XPath string for parsing.
 * @nattr: The number of attributes in the current node.
 * @attribs: The attributes of the current XML node.
 *
 * The #GgnAboutDialog is a simple object used to display information
 * relating to development and licensing of the notifier. See the
 * #GtkAboutDialog widget for more information on the premise of the
 * about dialog in Gnome.
 **/
struct _GgnXMLPrivate {
  /* the libxml2 parser. */
  xmlTextReader *reader;

  /* current node information. */
  gint type;
  gchar *name;
  gchar *text;

  /* a non-conforming XPath string */
  gchar *xpath;

  /* attribute information. */
  gint nattr;
  GHashTable *attribs;
};

