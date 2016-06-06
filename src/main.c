/**
 * @file src/main.c Gnome Gmail Notifier main source
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
 * SECTION:Main
 * @short_description: The main source file
 * @include: src/main.h
 **/

/*
 * include our application header.
 */
#include "main.h"

/**
 * main:
 * @argc: Number of command-line arguments.
 * @argv: String array of command-line arguments.
 *
 * This function acts as the application's entry point,
 * starting the basic GTK/GThread/GObject framework
 * before punting to a newly created GgnManager.
 *
 * Returns: application exit status.
 **/
int main (int argc, char **argv) {
#ifdef ENABLE_NLS
  /* setup gettext before anything else. */
  bindtextdomain (GETTEXT_PACKAGE, GGN_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif /* ENABLE_NLS */

  /* start glib threads. */
  if (!g_thread_supported ()) {
    /* start it up! */
    g_thread_init (NULL);
  }

  /* start gdk threads. */
  gdk_threads_init ();

  /* lock and start gtk. */
  gtk_init (&argc, &argv);

  /* start gstreamer. */
  gst_init (&argc, &argv);

  /* create a manager object. */
  GgnManager *manager = ggn_manager_new ();

  /* punt control to the manager. */
  ggn_manager_main (manager, argc, argv);

  /* exit the application. */
  return 0;
}

/**
 * ggn_link_execute:
 * @link: the complete URI of the hyperlink to be executed.
 *
 * Executes an internet/mail client application after
 * determining the URI prefix, which ultimately
 * determines which client to open.
 *
 * Returns: void.
 **/
void ggn_link_execute (const gchar *link) {
  /* declare helping variables. */
  gchar *prog, *execute, *elink;
  GConfClient *conf;

  /* get the default gconf client. */
  conf = gconf_client_get_default ();

  /* see what our URI prefix is. */
  if ((g_str_has_prefix (link, "http") == TRUE) ||
      (g_str_has_prefix (link, "file") == TRUE)) {
    /* load the web browser. */
    prog = (gchar *) gconf_client_get_string (conf, GGN_GCONF_BROWSER, NULL);

    /* use the link as-is. */
    elink = g_strdup (link);
  }
  else {
    /* load the email client. */
    prog = (gchar *) gconf_client_get_string (conf, GGN_GCONF_EMAILER, NULL);

    /* prefix the link with mailto: */
    elink = g_strdup_printf ("mailto:%s", link);
  }

  /* build the final string. */
  execute = g_strdup_printf (prog, elink);

  /* run the command. */
  g_spawn_command_line_async (execute, NULL);

  /* unref the gconf client. */
  g_object_unref (G_OBJECT (conf));

  /* free the built strings. */
  g_free (execute);
  g_free (elink);
  g_free (prog);
}

