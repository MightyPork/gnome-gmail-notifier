
defaultdir = $(datadir)/gnome-control-center/default-apps
default_DATA = gnome-gmail-notifier.xml

desktopdir = $(datadir)/applications
desktop_in_files = gnome-gmail-notifier.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

pkgdata_in_files = ggn-about.xml.in \
                   ggn-prefs.xml.in \
                   ggn-edit.xml.in \
                   ggn-menu.xml.in
pkgdata_DATA = $(pkgdata_in_files:.xml.in=.xml)

pixmapsdir = $(pkgdatadir)
pixmaps_DATA = ggn-new-lg.svg \
               ggn-new-sm.svg \
               ggn-error-lg.svg \
               ggn-error-sm.svg \
               ggn-normal-lg.svg \
               ggn-normal-sm.svg \
               ggn-wait.svg \
               ggn-warn.svg \
               ggn-write.svg

iconsdir = $(datadir)/pixmaps
icons_DATA = gnome-gmail-notifier.svg

EXTRA_DIST = $(default_DATA) \
             $(desktop_in_files) \
             $(pkgdata_in_files) \
             $(pixmaps_DATA) \
             $(icons_DATA)

CLEANFILES = $(desktop_DATA) \
             $(pkgdata_DATA)

DISTCLEANFILES = $(desktop_DATA) \
                 $(pkgdata_DATA)
