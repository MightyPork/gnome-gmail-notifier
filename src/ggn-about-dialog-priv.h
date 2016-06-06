/**
 * @file src/ggn-about-dialog-priv.h The GgnAboutDialog object
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
 * GgnAboutDialogPrivate:
 * @xml: The #GtkBuilder xml gui object.
 * @dlg: The #GtkAboutDialog widget.
 *
 * The #GgnAboutDialog is a simple object used to display information
 * relating to development and licensing of the notifier. See the
 * #GtkAboutDialog widget for more information on the premise of the
 * about dialog in Gnome.
 **/
struct _GgnAboutDialogPrivate {
  /* our dialog object. */
  GtkBuilder *xml;
  GtkAboutDialog *dlg;
};

