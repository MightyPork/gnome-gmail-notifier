AC_PREREQ(2.52)

AC_INIT(gnome-gmail-notifier, 0.10.1, geekysuavo@gmail.com)
AC_CONFIG_SRCDIR(src/main.c)
AM_CONFIG_HEADER(src/config.h)

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_INTLTOOL([0.35.0])
AC_STDC_HEADERS

GETTEXT_PACKAGE=$PACKAGE_NAME
GGN_LOCALEDIR="${datadir}/locale"
AC_SUBST(GETTEXT_PACKAGE)
AC_SUBST(GGN_LOCALEDIR)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

ALL_LINGUAS="es"
AM_GLIB_GNU_GETTEXT

GLIB_REQUIRED=2.6.0
GTK_REQUIRED=2.12.0
GCONF_REQUIRED=2.20.0
LIBNOTIFY_REQUIRED=0.4.2
LIBSOUP_REQUIRED=2.4.0
LIBXML2_REQUIRED=2.6.20
GSTREAMER_REQUIRED=0.10.0
GNOME_KEYRING_REQUIRED=0.4.2

AC_SUBST(GLIB_REQUIRED)
AC_SUBST(GTK_REQUIRED)
AC_SUBST(GCONF_REQUIRED)
AC_SUBST(LIBNOTIFY_REQUIRED)
AC_SUBST(LIBSOUP_REQUIRED)
AC_SUBST(LIBXML2_REQUIRED)
AC_SUBST(GSTREAMER_REQUIRED)
AC_SUBST(GNOME_KEYRING_REQUIRED)

PKG_CHECK_MODULES(GGN, glib-2.0 >= $GLIB_REQUIRED
                       gobject-2.0 gthread-2.0
                       gtk+-2.0 >= $GTK_REQUIRED
                       gconf-2.0 >= $GCONF_REQUIRED
                       libnotify >= $LIBNOTIFY_REQUIRED
                       libsoup-2.4 >= $LIBSOUP_REQUIRED
                       libxml-2.0 >= $LIBXML2_REQUIRED
                       gstreamer-0.10 >= $GSTREAMER_REQUIRED
                       gnome-keyring-1 >= $GNOME_KEYRING_REQUIRED)

AC_SUBST(GGN_CFLAGS)
AC_SUBST(GGN_LIBS)

AC_PATH_PROG(GST_INSPECT, gst-inspect-0.10, no)
if test x$GST_INSPECT = xno; then
    AC_MSG_ERROR([Could not find a gst-inspect in your PATH])
fi
AC_SUBST(GST_INSPECT)

AC_DEFUN([GGN_CHECK_GSTREAMER_PLUGIN],
[
    AC_MSG_CHECKING([for gstreamer $1 plugin])
    if $GST_INSPECT $1 >/dev/null 2>/dev/null; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([Could not find gstreamer plugin '$element'])
    fi
])

GGN_CHECK_GSTREAMER_PLUGIN([filesrc])
GGN_CHECK_GSTREAMER_PLUGIN([wavparse])
GGN_CHECK_GSTREAMER_PLUGIN([audioconvert])
GGN_CHECK_GSTREAMER_PLUGIN([audioresample])
GGN_CHECK_GSTREAMER_PLUGIN([autoaudiosink])

AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal, no)
if test x$GLIB_GENMARSHAL = xno; then
    AC_MSG_ERROR([Could not find a glib-genmarshal in your PATH])
fi
AC_SUBST(GLIB_GENMARSHAL)

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 po/Makefile.in
                 data/Makefile
                 data/gnome-gmail-notifier.desktop
                 data/ggn-about.xml
                 data/ggn-prefs.xml
                 data/ggn-edit.xml
                 data/ggn-menu.xml])
AC_CONFIG_COMMANDS([default], [[ echo timestamp > stamp-h ]], [[]])
AC_OUTPUT
