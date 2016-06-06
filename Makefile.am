SUBDIRS = src po data

EXTRA_DIST = autogen.sh \
             intltool-extract.in \
             intltool-merge.in \
             intltool-update.in \
             LICENSE

DISTCLEANFILES = intltool-extract \
                 intltool-merge \
                 intltool-update

fixme:
	grep -n -e fixme -e FIXME src/*.[ch]

checksplat:
	grep '* ' -n src/*.[ch] | sed -e '/\/\*/d' -e '/ \* /d'

