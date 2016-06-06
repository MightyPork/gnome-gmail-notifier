/**
 * @file src/ggn-xml.c The GgnXML object
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
 * SECTION:GgnXML
 * @short_description: The xml parser object
 * @include: src/ggn-xml.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-xml-priv.h"

/*
 * forward function definitions.
 */
static void ggn_xml_init (GgnXML *self);
static void ggn_xml_class_init (GgnXMLClass *klass);
static void ggn_xml_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnXML, ggn_xml, G_TYPE_OBJECT);

/*
 * define the xml node types.
 */
#define GGN_XML_NODE_TYPE_TEXT          3
#define GGN_XML_NODE_TYPE_OPENTAG       1
#define GGN_XML_NODE_TYPE_CLOSETAG      15

/*
 * define the signals used.
 */
enum {
  BEGIN_ELEMENT,
  END_ELEMENT,
  TEXT,
  LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };

/**
 * ggn_xml_default_begin_element_cb:
 * @xml: The xml parser which emitted the signal.
 * @element_path: The "xpath" of the element parsed.
 * @element_name: The name of the element parsed.
 * @attributes: The attributes of the element.
 *
 * This is the default "begin_element" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_xml_default_begin_element_cb (GgnXML *xml,
                                              gchar *element_path,
                                              gchar *element_name,
                                              GHashTable *attributes) {
  /* do nothing. */
}

/**
 * ggn_xml_default_end_element_cb:
 * @xml: The xml parser which emitted the signal.
 * @element_path: The "xpath" of the element parsed.
 * @element_name: The name of the element parsed.
 *
 * This is the default "end_element" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_xml_default_end_element_cb (GgnXML *xml,
                                            gchar *element_path,
                                            gchar *element_name) {
  /* do nothing. */
}

/**
 * ggn_xml_default_text_cb:
 * @xml: The xml parser which emitted the signal.
 * @element_path: The "xpath" of the current element.
 * @element_name: The name of the current element.
 * @value: The text we wave parsed from the XML.
 *
 * This is the default "text" callback function manager.
 *
 * Returns: void.
 **/
static void ggn_xml_default_text_cb (GgnXML *xml,
                                     gchar *element_path,
                                     gchar *element_name,
                                     gchar *value) {
  /* do nothing. */
}

/*
 * ggn_xml_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_xml_init (GgnXML *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnXMLPrivate, 1);

  /* setup private instance objects. */
  self->priv->reader = NULL;
}

/*
 * ggn_xml_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_xml_class_init (GgnXMLClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_xml_finalize;

  /* setup the default signal handler. */
  klass->begin_element = ggn_xml_default_begin_element_cb;
  klass->end_element = ggn_xml_default_end_element_cb;
  klass->text = ggn_xml_default_text_cb;

  /**
   * GgnXML::begin_element:
   * @xml: The xml parser which emitted the signal.
   * @element_path: The pseudo-XPath for the parsed node.
   * @element_name: The tag name for the parsed node.
   * @attributes: The attributes inside the parsed node.
   *
   * Emitted when the parser encounters an opening XML tag.
   **/
  signals[BEGIN_ELEMENT] = 
    g_signal_new ("begin_element",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnXMLClass, begin_element),
                  NULL, NULL,
                  ggn_marshal_VOID__STRING_STRING_OBJECT,
                  G_TYPE_NONE, 3,
                  G_TYPE_STRING,
                  G_TYPE_STRING,
                  G_TYPE_HASH_TABLE);

  /**
   * GgnXML::end_element:
   * @xml: The xml parser which emitted the signal.
   * @element_path: The pseudo-XPath for the parsed node.
   * @element_name: The tag name for the parsed node.
   *
   * Emitted when the parser encounters a closing XML tag.
   **/
  signals[END_ELEMENT] =
    g_signal_new ("end_element",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnXMLClass, end_element),
                  NULL, NULL,
                  ggn_marshal_VOID__STRING_STRING,
                  G_TYPE_NONE, 2,
                  G_TYPE_STRING,
                  G_TYPE_STRING);

  /**
   * GgnXML::text:
   * @xml: The xml parser which emitted the signal.
   * @element_path: The pseudo-XPath for the parsed node.
   * @element_name: The tag name for the parsed node.
   * @value: The textual content that we've parsed.
   *
   * Emitted when the parser encounters textual content of some kind.
   **/
  signals[TEXT] =
    g_signal_new ("text",
                  G_OBJECT_CLASS_TYPE (gobj_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GgnXMLClass, text),
                  NULL, NULL,
                  ggn_marshal_VOID__STRING_STRING_STRING,
                  G_TYPE_NONE, 3,
                  G_TYPE_STRING,
                  G_TYPE_STRING,
                  G_TYPE_STRING);
}

/*
 * ggn_xml_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_xml_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnXML *self = GGN_XML (obj);

  /* free the private instance objects. */

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_xml_parent_class)->finalize (obj);
}

/**
 * ggn_xml_new:
 *
 * Creates a new #GgnXML with default values, which are
 * used to parse any XML-based strings.
 *
 * Returns: the new #GgnXML object. Free with ggn_xml_free().
 **/
GgnXML *ggn_xml_new (void) {
  /* make a newly created gobject. */
  GgnXML *xml = g_object_new (GGN_TYPE_XML, NULL);

  /* return the new object. */
  return xml;
}

/**
 * ggn_xml_free:
 * @xml: The #GgnXML to free.
 *
 * Frees the given #GgnXML object by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_xml_free (GgnXML *xml) {
  /* while this object exists... */
  while (G_IS_OBJECT (xml)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (xml));
  }
}

/**
 * ggn_xml_generate_xpath:
 * @xml: The XML parser to build an XPath from.
 *
 * This function builds a totally non-conforming XPath expression from
 * an xmlTextReader object by extracting the current xmlNode and then
 * working backwards to the document node.
 *
 * Returns: the pseudo-XPath string. Free with g_free().
 **/
gchar *ggn_xml_generate_xpath (GgnXML *xml) {
  /* declare our helping variables. */
  gchar *rev, *str;
  xmlNode *node;
  GString *gstr;

  /* allocate memory for the gstring. */
  gstr = g_string_new ("");

  /* get the current xml node and add it to the string. */
  node = xmlTextReaderCurrentNode (xml->priv->reader);
  rev = g_utf8_strreverse ((gchar *) node->name, -1);
  g_string_append_printf (gstr, "%s/", rev);
  g_free (rev);

  /* loop while there are still parents. */
  node = node->parent;
  while (node->name != NULL) {
    /* append the node name to the string. */
    rev = g_utf8_strreverse ((gchar*) node->name, -1);
    g_string_append_printf (gstr, "%s/", rev);
    g_free (rev);

    /* get the new parent node. */
    node = node->parent;
  }

  /* create a new string from our GString. */
  str = g_utf8_strreverse (gstr->str, -1);
  g_string_free (gstr, TRUE);

  /* return the new string. */
  return str;
}

/**
 * ggn_xml_load_string:
 * @xml: An XML parser to parse the string.
 * @str: The XML UTF-8 text to parse.
 *
 * Loads an XML subset from a string in memory, as represented by
 * a gchar* variable, and then begins to parse the XML.
 *
 * Returns: success boolean.
 **/
gboolean ggn_xml_load_string (GgnXML *xml, const gchar *str) {
  /* set up the reader for parsing. */
  xml->priv->reader = xmlReaderForDoc ((xmlChar *) str, NULL, "UTF-8",
                                       XML_PARSE_RECOVER | XML_PARSE_NOERROR);

  /* allocate the attributes hash table. */
  xml->priv->attribs = g_hash_table_new (g_str_hash, g_str_equal);

  /* loop and continue parsing. */
  while (xmlTextReaderRead (xml->priv->reader) == 1) {
    /* determine the xml node type. */
    xml->priv->type = xmlTextReaderNodeType (xml->priv->reader);

    /* act variably by the node type. */
    if (xml->priv->type == GGN_XML_NODE_TYPE_OPENTAG) {
      /* get the number of attributes. */
      xml->priv->nattr = xmlTextReaderAttributeCount (xml->priv->reader);

      /* get the node name. */
      xml->priv->name = (gchar *) xmlTextReaderConstName (xml->priv->reader);

      /* get a non-conforming xpath string. */
      xml->priv->xpath = ggn_xml_generate_xpath (xml);

      /* reset the hash table. */
      g_hash_table_remove_all (xml->priv->attribs);

      /* do we have attributes? */
      if (xml->priv->nattr > 0) {
        /* read the attributes into the hash table. */
        while (xmlTextReaderMoveToNextAttribute (xml->priv->reader)) {
          /* add the strings to our hash table. */
          g_hash_table_insert (xml->priv->attribs,
                     (gchar *) xmlTextReaderConstName (xml->priv->reader),
                     (gchar *) xmlTextReaderConstValue (xml->priv->reader));
        }
      }

      /* emit the "begin_element" signal. */
      g_signal_emit (xml, signals[BEGIN_ELEMENT], 0,
                     xml->priv->xpath,
                     xml->priv->name,
                     xml->priv->attribs);

      /* reset our attribute count for the next tag. */
      xml->priv->nattr = 0;

      /* free the string we allocated. */
      g_free (xml->priv->xpath);
      
    }
    else if (xml->priv->type == GGN_XML_NODE_TYPE_CLOSETAG) {
      /* get the node name. */
      xml->priv->name = (gchar *) xmlTextReaderConstName (xml->priv->reader);

      /* get a non-conforming xpath string. */
      xml->priv->xpath = ggn_xml_generate_xpath (xml);

      /* emit the "end_element" signal. */
      g_signal_emit (xml, signals[END_ELEMENT], 0,
                     xml->priv->xpath,
                     xml->priv->name);

      /* free the string we allocated. */
      g_free (xml->priv->xpath);
    }
    else if (xml->priv->type == GGN_XML_NODE_TYPE_TEXT) {
      /* get the node name. */
      xml->priv->name = (gchar *) xmlTextReaderConstName (xml->priv->reader);

      /* get a non-conforming xpath string. */
      xml->priv->xpath = ggn_xml_generate_xpath (xml);

      /* get the content text. */
      xml->priv->text = (gchar *) xmlTextReaderReadString (xml->priv->reader);

      /* emit the "text" signal. */
      g_signal_emit (xml, signals[TEXT], 0,
                     xml->priv->xpath,
                     xml->priv->name,
                     xml->priv->text);

      /* free the strings we allocated. */
      g_free (xml->priv->xpath);
      g_free (xml->priv->text);
    }

    /* reset our integer values. */
    xml->priv->type = 0;
  }

  /* free the attributes hash table. */
  g_hash_table_destroy (xml->priv->attribs);

  /* free the reader. */
  xmlFreeTextReader (xml->priv->reader);

  /* set the exit status. */
  return TRUE;
}

/**
 * ggn_xml_load_file:
 * @xml: The XML parser to parse the file.
 * @filename: The filename of the XML to parse.
 *
 * Loads an XML subset from an XML file, as represented by
 * a gchar* variable, and then begins to parse the XML.
 *
 * Returns: success boolean.
 **/
gboolean ggn_xml_load_file (GgnXML *xml, const gchar *filename) {
  /* declare some helping variables. */
  gboolean status;
  gchar *body;

  /* make sure the file exists. */
  if (!g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
    /* exit the function. */
    return FALSE;
  }

  /* read in the file's contents.. */
  if (!g_file_get_contents (filename, &body, NULL, NULL)) {
    /* exit the function. */
    return FALSE;
  }

  /* run the string parsing function. */
  status = ggn_xml_load_string (xml, body);

  /* free the string. */
  g_free (body);

  /* set the exit status. */
  return status;
}

