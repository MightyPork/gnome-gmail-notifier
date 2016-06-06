/**
 * @file src/ggn-prefs.h The GgnPrefs object
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
#ifndef __GNOME_GMAIL_NOTIFIER_PREFS_H__
#define __GNOME_GMAIL_NOTIFIER_PREFS_H__

/*
 * our header is just beginning.
 */
G_BEGIN_DECLS

/*
 * gconf preference keys specific to the notifier.
 */
#define GGN_PREF_GLOBAL_ROOT       "/apps/gnome-gmail-notifier/global"
#define GGN_PREF_ACCOUNT_ROOT      "/apps/gnome-gmail-notifier/accounts"
#define GGN_PREF_NOTIFY_NEW        GGN_PREF_GLOBAL_ROOT "/notify_new"
#define GGN_PREF_NOTIFY_ERR        GGN_PREF_GLOBAL_ROOT "/notify_err"
#define GGN_PREF_SOUND_ENAB        GGN_PREF_GLOBAL_ROOT "/sound_enab"
#define GGN_PREF_SOUND_FILE        GGN_PREF_GLOBAL_ROOT "/sound_file"
#define GGN_PREF_CHECKRATE         GGN_PREF_GLOBAL_ROOT "/checkrate"
#define GGN_PREF_FIRSTRUN          GGN_PREF_GLOBAL_ROOT "/firstrun"
#define GGN_PREF_ACCOUNT_LIST      GGN_PREF_GLOBAL_ROOT "/account_list"
#define GGN_PREF_ACCOUNT_DESC      GGN_PREF_ACCOUNT_ROOT "/%s/name"
#define GGN_PREF_ACCOUNT_USER      GGN_PREF_ACCOUNT_ROOT "/%s/user"
#define GGN_PREF_ACCOUNT_DOMAIN    GGN_PREF_ACCOUNT_ROOT "/%s/domain"
#define GGN_PREF_ACCOUNT_ENABLED   GGN_PREF_ACCOUNT_ROOT "/%s/enab"
#define GGN_PREF_ACCOUNT_DEFAULT   GGN_PREF_ACCOUNT_ROOT "/%s/def"

/*
 * gconf preference keys nonspecific to the notifier.
 */
#define GGN_GCONF_BROWSER           "/desktop/gnome/url-handlers/http/command"
#define GGN_GCONF_EMAILER           "/desktop/gnome/url-handlers/mailto/command"
#define GGN_GCONF_PROXY_USE         "/system/http_proxy/use_http_proxy"
#define GGN_GCONF_PROXY_HOST        "/system/http_proxy/host"
#define GGN_GCONF_PROXY_PORT        "/system/http_proxy/port"
#define GGN_GCONF_PROXY_SSL_HOST    "/system/proxy/secure_host"
#define GGN_GCONF_PROXY_SSL_PORT    "/system/proxy/secure_port"
#define GGN_GCONF_PROXY_USER        "/system/http_proxy/authentication_user"
#define GGN_GCONF_PROXY_PASS        "/system/http_proxy/authentication_password"

/*
 * object function macros.
 */
#define GGN_TYPE_PREFS (ggn_prefs_get_type ())
#define GGN_PREFS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GGN_TYPE_PREFS, GgnPrefs))
#define GGN_PREFS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GGN_TYPE_PREFS, GgnPrefsClass))
#define GGN_IS_PREFS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GGN_TYPE_PREFS))
#define GGN_IS_PREFS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GGN_TYPE_PREFS))
#define GGN_PREFS_GET_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), GGN_TYPE_PREFS, GgnPrefsClass))

/*
 * type macros.
 */
typedef struct _GgnPrefs GgnPrefs;
typedef struct _GgnPrefsClass GgnPrefsClass;
typedef struct _GgnPrefsPrivate GgnPrefsPrivate;

/*
 * object definition.
 */
struct _GgnPrefs {
  /* parent gobject class. */
  GObject parent_object;

  /* private instance members. */
  GgnPrefsPrivate *priv;
};

/*
 * class definition.
 */
struct _GgnPrefsClass {
  /* parent gobject class. */
  GObjectClass parent;
};

/*
 * gobject-type definition.
 */
GType ggn_prefs_get_type (void);

/*
 * object creation functions.
 */
GgnPrefs *ggn_prefs_new (void);
void ggn_prefs_free (GgnPrefs *prefs);

/*
 * integer access functions.
 */
gint ggn_prefs_get_int (GgnPrefs *prefs, const gchar *key);
void ggn_prefs_set_int (GgnPrefs *prefs, const gchar *key, gint value);

/*
 * boolean access functions.
 */
gboolean ggn_prefs_get_bool (GgnPrefs *prefs, const gchar *key);
void ggn_prefs_set_bool (GgnPrefs *prefs, const gchar *key, gboolean value);

/*
 * linked list access functions.
 */
GSList *ggn_prefs_get_list (GgnPrefs *prefs, const gchar *key);
void ggn_prefs_set_list (GgnPrefs *prefs, const gchar *key, GSList *list);

/*
 * string access functions.
 */
gchar *ggn_prefs_get_string (GgnPrefs *prefs, const gchar *key);
void ggn_prefs_set_string (GgnPrefs *prefs,
                           const gchar *key,
                           const gchar *value);

/*
 * tree access functions.
 */
void ggn_prefs_unset (GgnPrefs *prefs, const gchar *key);

/*
 * our header is at its end.
 */
G_END_DECLS

#endif /* __GNOME_GMAIL_NOTIFIER_PREFS_H__ */

