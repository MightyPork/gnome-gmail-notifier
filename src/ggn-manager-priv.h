/**
 * @file src/ggn-manager-priv.h The GgnManager object
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
 * GgnManagerPrivate:
 * @prefs_win: The preferences window we use.
 * @about_dlg: The about dialog we use.
 * @icon: The status icon in the panel.
 * @prefs: The globally used preferences object.
 * @audio: The audio playback #GObject to use.
 * @alist: The managed accounts listing.
 *
 * The #GgnManager is a simple GObject that coordinates the
 * higher-level functionality of the gnome gmail notifier
 * application.
 **/
struct _GgnManagerPrivate {
  /* the gtk elements of the manager. */
  GgnPrefsWindow *prefs_win;
  GgnAboutDialog *about_dlg;

  /* the status icon. */
  GgnIcon *icon;

  /* the preferences object we use. */
  GgnPrefs *prefs;

  /* the audio playback object. */
  GgnAudio *audio;

  /* the accounts listing. */
  GgnAccountList *alist;
};

