/**
 * @file src/ggn-xml.h GgnXML object
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
#ifndef __GNOME_GMAIL_NOTIFIER_XML_H__
#define __GNOME_GMAIL_NOTIFIER_XML_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * object function macros.
 */
#define GGN_TYPE_XML (ggn_xml_get_type ())
#define GGN_XML(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_XML, GgnXML))
#define GGN_XML_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_XML, GgnXMLClass))
#define GGN_IS_XML(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_XML))
#define GGN_IS_XML_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_XML))
#define GGN_XML_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_XML, GgnXMLClass))

/*
 * type macros.
 */
typedef struct _GgnXML GgnXML;
typedef struct _GgnXMLClass GgnXMLClass;
typedef struct _GgnXMLPrivate GgnXMLPrivate;

/*
 * object definition.
 */
struct _GgnXML {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnXMLPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnXMLClass {
  /* parent gobject class. */
  GObjectClass parent;

  /* "begin_element": called on opened tags. */
  void (*begin_element) (GgnXML *xml,
                         gchar *element_path,
                         gchar *element_name,
                         GHashTable *attributes);

  /* "end_element": called on closed tags. */
  void (*end_element) (GgnXML *xml,
                       gchar *element_path,
                       gchar *element_name);

  /* "text": called on text content. */
  void (*text) (GgnXML *xml,
                gchar *element_path,
                gchar *element_name,
                gchar *value);
};

/*
 * gobject-type definition.
 */
GType ggn_xml_get_type (void);

/*
 * object creation functions.
 */
GgnXML *ggn_xml_new (void);
void ggn_xml_free (GgnXML *xml);

/*
 * functions to parse various forms of text.
 */
gboolean ggn_xml_load_string (GgnXML *xml, const gchar *str);
gboolean ggn_xml_load_file (GgnXML *xml, const gchar *filename);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_XML_H__ */

