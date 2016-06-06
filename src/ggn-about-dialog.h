/**
 * @file src/ggn-about-dialog.h GgnAboutDialog object
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
#ifndef __GNOME_GMAIL_NOTIFIER_ABOUT_DIALOG_H__
#define __GNOME_GMAIL_NOTIFIER_ABOUT_DIALOG_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * object function macros.
 */
#define GGN_TYPE_ABOUT_DIALOG (ggn_about_dialog_get_type ())
#define GGN_ABOUT_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_ABOUT_DIALOG, GgnAboutDialog))
#define GGN_ABOUT_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_ABOUT_DIALOG, GgnAboutDialogClass))
#define GGN_IS_ABOUT_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_ABOUT_DIALOG))
#define GGN_IS_ABOUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_ABOUT_DIALOG))
#define GGN_ABOUT_DIALOG_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_ABOUT_DIALOG, GgnAboutDialogClass))

/*
 * type macros.
 */
typedef struct _GgnAboutDialog GgnAboutDialog;
typedef struct _GgnAboutDialogClass GgnAboutDialogClass;
typedef struct _GgnAboutDialogPrivate GgnAboutDialogPrivate;

/*
 * object definition.
 */
struct _GgnAboutDialog {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnAboutDialogPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnAboutDialogClass {
  /* parent gobject class. */
  GObjectClass parent;

  /* signals. */
};

/*
 * gobject-type definition.
 */
GType ggn_about_dialog_get_type (void);

/*
 * object creation functions.
 */
GgnAboutDialog *ggn_about_dialog_new (void);
void ggn_about_dialog_free (GgnAboutDialog *dialog);

/*
 * show/hide functions.
 */
void ggn_about_dialog_show (GgnAboutDialog *dialog);
void ggn_about_dialog_hide (GgnAboutDialog *dialog);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_ABOUT_DIALOG_H__ */

