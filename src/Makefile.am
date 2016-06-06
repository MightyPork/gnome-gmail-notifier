
bin_PROGRAMS = gnome-gmail-notifier

EXTRA_DIST = ggn-marshal.list

MARSHAL_GENERATED = ggn-marshal.c \
                    ggn-marshal.h

ggn-marshal.h: ggn-marshal.list
	( @GLIB_GENMARSHAL@ --prefix=ggn_marshal $(srcdir)/ggn-marshal.list --header > ggn-marshal.tmp \
	&& mv ggn-marshal.tmp ggn-marshal.h ) || ( rm -f ggn-marshal.tmp && exit 1 )

ggn-marshal.c: ggn-marshal.h
	( (echo '#include "ggn-marshal.h"'; @GLIB_GENMARSHAL@ --prefix=ggn_marshal $(srcdir)/ggn-marshal.list --body) > ggn-marshal.tmp \
	&& mv ggn-marshal.tmp ggn-marshal.c ) || ( rm -f ggn-marshal.tmp && exit 1 )

BUILT_SOURCES = $(MARSHAL_GENERATED)

CLEANFILES = $(MARSHAL_GENERATED)

gnome_gmail_notifier_SOURCES = main.c \
                               main.h \
                               $(MARSHAL_GENERATED) \
                               ggn-about-dialog.c \
                               ggn-about-dialog.h \
                               ggn-about-dialog-priv.h \
                               ggn-audio.c \
                               ggn-audio.h \
                               ggn-audio-priv.h \
                               ggn-keyring.c \
                               ggn-keyring.h \
                               ggn-prefs.c \
                               ggn-prefs.h \
                               ggn-prefs-priv.h \
                               ggn-icon.c \
                               ggn-icon.h \
                               ggn-icon-priv.h \
                               ggn-xml.c \
                               ggn-xml.h \
                               ggn-xml-priv.h \
                               ggn-entry.c \
                               ggn-entry.h \
                               ggn-entry-priv.h \
                               ggn-account.c \
                               ggn-account.h \
                               ggn-account-priv.h \
                               ggn-account-list.c \
                               ggn-account-list.h \
                               ggn-account-list-priv.h \
                               ggn-prefs-window.c \
                               ggn-prefs-window.h \
                               ggn-prefs-window-priv.h \
                               ggn-manager.c \
                               ggn-manager.h \
                               ggn-manager-priv.h

gnome_gmail_notifier_LDFLAGS = -Wl,--export-dynamic

gnome_gmail_notifier_LDADD = $(GGN_LIBS)

AM_CPPFLAGS = $(GGN_CFLAGS) -Wall -Wl,--export-dynamic \
              -DPIXMAPS_DIR=\""$(pkgdatadir)"\" \
              -DXMLUI_DIR=\""$(pkgdatadir)"\" \
              -DGETTEXT_PACKAGE=\""$(GETTEXT_PACKAGE)"\" \
              -DGGN_LOCALEDIR=\""$(GGN_LOCALEDIR)"\"

fixme:
	grep -n -e fixme -e FIXME *.[ch]

again: clean all
