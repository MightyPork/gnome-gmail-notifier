/**
 * @file src/ggn-about-dialog.c GgnAboutDialog object
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
 * SECTION:GgnAboutDialog
 * @short_description: The about dialog object
 * @include: src/ggn-about-dialog.h
 **/

/*
 * include our application header.
 */
#include <main.h>
#include "ggn-about-dialog-priv.h"

/*
 * forward function definitions for glib.
 */
static void ggn_about_dialog_init (GgnAboutDialog *self);
static void ggn_about_dialog_class_init (GgnAboutDialogClass *klass);
static void ggn_about_dialog_finalize (GObject *obj);

/*
 * define the gobject type and its basic functions.
 */
G_DEFINE_TYPE (GgnAboutDialog, ggn_about_dialog, G_TYPE_OBJECT);

/**
 * ggn_about_dialog_link:
 * @about: the #GtkAboutDialog we're using.
 * @link: the link that was executed.
 * @data: callback data we can use.
 *
 * Callback function that is executed when the user clicks a
 * hyperlink inside the about dialog. When called, this function
 * opens the executable required to view the hyperlink.
 *
 * Returns: void.
 **/
static void ggn_about_dialog_link (GtkAboutDialog *about,
                                   const gchar *link,
                                   gpointer data) {
  /* execute the link passed to us. */
  ggn_link_execute (link);
}

/*
 * ggn_about_dialog_init:
 *
 * This function is used by the gobject library to
 * generate a new instance of our object.
 */
static void ggn_about_dialog_init (GgnAboutDialog *self) {
  /* set up the private data structure. */
  self->priv = g_new0 (GgnAboutDialogPrivate, 1);

  /* setup the about dialog hyperlink handler. */
  gtk_about_dialog_set_url_hook (ggn_about_dialog_link, self, NULL);
  gtk_about_dialog_set_email_hook (ggn_about_dialog_link, self, NULL);

  /* allocate a gtkbuilder object. */
  self->priv->xml = gtk_builder_new ();

  /* load the about dialog xml file. */
  gtk_builder_add_from_file (self->priv->xml, GGN_XMLUI_ABOUT, NULL);

  /* retrieve the about dialog widget. */
  self->priv->dlg = (GtkAboutDialog *) gtk_builder_get_object
                                         (self->priv->xml,
                                          "GgnAboutDialog");

  /* link the response callback to our dialog. */
  g_signal_connect (G_OBJECT (self->priv->dlg),
                    "response",
                    G_CALLBACK (gtk_widget_hide),
                    self);
}

/*
 * ggn_about_dialog_class_init:
 *
 * This function is used by the gobject library to
 * generate a new class object of our object.
 */
static void ggn_about_dialog_class_init (GgnAboutDialogClass *klass) {
  /* setup a gobject class. */
  GObjectClass *gobj_class = G_OBJECT_CLASS (klass);

  /* set the locations of our destruction function. */
  gobj_class->finalize = ggn_about_dialog_finalize;
}

/*
 * ggn_about_dialog_finalize:
 *
 * This function is used by the gobject library to cleanly finish
 * the destruction process started by the dispose function.
 */
static void ggn_about_dialog_finalize (GObject *obj) {
  /* make a reference to ourself. */
  GgnAboutDialog *self = GGN_ABOUT_DIALOG (obj);

  /* free the gtkbuilder xml and the dialog widget. */
  g_object_unref (G_OBJECT (self->priv->xml));
  gtk_widget_destroy (GTK_WIDGET (self->priv->dlg));

  /* destroy the private object. */
  g_free (self->priv);
  self->priv = NULL;

  /* chain up to the parent class. */
  G_OBJECT_CLASS (ggn_about_dialog_parent_class)->finalize (obj);
}

/**
 * ggn_about_dialog_new:
 *
 * Creates a new #GgnAboutDialog object for use in the application.
 * It simply allows the user to view information relating to the
 * development version, license, and developers of the notifier.
 *
 * Returns: A new #GgnAboutDialog object.
 **/
GgnAboutDialog *ggn_about_dialog_new (void) {
  /* make a newly created gobject. */
  GgnAboutDialog *dialog = g_object_new (GGN_TYPE_ABOUT_DIALOG, NULL);

  /* return the new object. */
  return dialog;
}

/**
 * ggn_about_dialog_free:
 * @dialog: the #GgnAboutDialog to free.
 *
 * Frees the given #GgnAboutDialog by decreasing its reference count
 * until the #GObject library is forced to free the memory allocated
 * for its use.
 *
 * Returns: void.
 **/
void ggn_about_dialog_free (GgnAboutDialog *dialog) {
  /* while this object exists... */
  while (G_IS_OBJECT (dialog)) {
    /* unreference this object. */
    g_object_unref (G_OBJECT (dialog));
  }
}

/**
 * ggn_about_dialog_show:
 * @dialog: the #GgnAboutDialog to show.
 *
 * Makes the provided about dialog visible by showing its associated
 * #GtkAboutDialog widget.
 *
 * Returns: void.
 **/
void ggn_about_dialog_show (GgnAboutDialog *dialog) {
  /* show the widget. */
  gtk_widget_show (GTK_WIDGET (dialog->priv->dlg));
}

/**
 * ggn_about_dialog_hide:
 * @dialog: the #GgnAboutDialog to hide.
 *
 * Makes the provided about dialog invisible by hiding its associated
 * #GtkAboutDialog widget.
 *
 * Returns: void.
 **/
void ggn_about_dialog_hide (GgnAboutDialog *dialog) {
  /* hide the widget. */
  gtk_widget_hide (GTK_WIDGET (dialog->priv->dlg));
}

