/**
 * @file src/main.h Gnome Gmail Notifier main source
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

#ifndef __GNOME_GMAIL_NOTIFIER_MAIN_H__
#define __GNOME_GMAIL_NOTIFIER_MAIN_H__

/*
 * ansi include statements. these files provide
 * basic ansi c functionality to the rest of the
 * application.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

/*
 * glib include statement. this includes the glib
 * "object-oriented" wrapper for gtk and other
 * special higher-level interfaces.
 */
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <glib-object.h>
#include <glib/gi18n.h>

/*
 * glib explicit threading support include statement.
 * this includes the gthread header and the pthread
 * header.
 */
#include <glib/gthread.h>
#include <pthread.h>

/*
 * gconf include statement. this includes the gconf
 * headers required to determine the gnome default
 * browser, so we can load gmail in a web browser
 * when the user clicks our icon.
 */
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

/*
 * gtk include statement. gtk provides the gui
 * for the rest of the application to use.
 */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/*
 * libnotify include statement. libnotify is used
 * to display notification "bubbles" for various
 * events, such as when the user recieves new mail
 * or the app cannot connect to the account.
 */
#include <libnotify/notify.h>

/*
 * libsoup include statement. libsoup provides the
 * interfaces required for downloading the atom
 * inbox feed from gmail using the http protocol.
 */
#include <libsoup/soup.h>

/*
 * libxml2 include statement. libxml2 provides
 * the xml parsing routines required to parse both
 * the ATOM 0.3 feed from GMail and the preferences
 * files for each user.
 */
#include <libxml/xmlreader.h>

/*
 * gstreamer include statement. gstreamer provides
 * a lower-level interface to the gnome sound play-
 * back systems and supports more than esd.
 */
#include <gst/gst.h>

/*
 * gnome-keyring include statement. this provides a
 * much more "gnome-like" method of saving password
 * data for each user account.
 */
#include <gnome-keyring.h>

/*
 * configuration include statements. the config.h
 * header is created during the ./configure process
 * to provide the application with some fairly
 * flexible defines.
 */
#include "config.h"

/*
 * the filenames for graphics and gtk xml files that the notifier
 * loads at runtime. these are configured by the autotools scripts.
 */
#define GGN_PIXMAP_NORMAL         PIXMAPS_DIR "/ggn-normal-lg.svg"
#define GGN_PIXMAP_NORMAL_SM      PIXMAPS_DIR "/ggn-normal-sm.svg"
#define GGN_PIXMAP_ERROR          PIXMAPS_DIR "/ggn-error-lg.svg"
#define GGN_PIXMAP_ERROR_SM       PIXMAPS_DIR "/ggn-error-sm.svg"
#define GGN_PIXMAP_MESSAGE        PIXMAPS_DIR "/ggn-new-lg.svg"
#define GGN_PIXMAP_MESSAGE_SM     PIXMAPS_DIR "/ggn-new-sm.svg"
#define GGN_PIXMAP_WARNING        PIXMAPS_DIR "/ggn-warn.svg"
#define GGN_PIXMAP_CHECKING       PIXMAPS_DIR "/ggn-wait.svg"
#define GGN_PIXMAP_COMPOSE        PIXMAPS_DIR "/ggn-write.svg"
#define GGN_XMLUI_MENU            XMLUI_DIR "/ggn-menu.xml"
#define GGN_XMLUI_PREFS           XMLUI_DIR "/ggn-prefs.xml"
#define GGN_XMLUI_ABOUT           XMLUI_DIR "/ggn-about.xml"
#define GGN_XMLUI_EDIT            XMLUI_DIR "/ggn-edit.xml"

/*
 * application header include statements. these
 * include all of the definitions for internal
 * program functionality.
 */
#include "ggn-marshal.h"
#include "ggn-about-dialog.h"
#include "ggn-audio.h"
#include "ggn-keyring.h"
#include "ggn-prefs.h"
#include "ggn-icon.h"
#include "ggn-xml.h"
#include "ggn-entry.h"
#include "ggn-account.h"
#include "ggn-account-list.h"
#include "ggn-manager.h"
#include "ggn-prefs-window.h"

/*
 * functions that are contained in no specific object, but are
 * essential to the operation of the application...
 */
void ggn_link_execute (const gchar *link);

#endif /* __GNOME_GMAIL_NOTIFIER_MAIN_H__ */

