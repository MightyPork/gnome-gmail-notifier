#!/bin/sh
# Run this to generate all the initial makefiles, etc.

DIE=0

PACKAGE=gnome-gmail-notifier

echo "Generating configuration files for $PACKAGE, please wait..."

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have autoconf installed to compile $PACKAGE."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at:"
    echo
    echo "    ftp://ftp.gnu.org/pub/gnu/autoconf/"
    DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have automake installed to compile $PACKAGE."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at:"
    echo
    echo "    ftp://ftp.gnu.org/pub/gnu/automake/"
    DIE=1
}

(glib-gettextize --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have glib-gettextize installed to compile $PACKAGE."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at:"
    echo
    echo "    ftp://ftp.gnu.org/pub/glib/"
    DIE=1
}

(intltoolize --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have intltoolize installed to compile $PACKAGE."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at:"
    echo
    echo "    ftp://ftp.gnome.org/pub/gnome/sources/intltool/"
    DIE=1
}

[ $DIE -eq 1 ] && exit 1;

echo "  intltoolize --copy --force --automake"
intltoolize --copy --automake --force
sed -e 's/^\(EXTRA_DISTFILES.*\)/\1 gnome-gmail-notifier.pot/g' -i po/Makefile.in.in
echo "  aclocal $ACLOCAL_FLAGS"
aclocal $ACLOCAL_FLAGS
echo "  autoheader"
autoheader
echo "  automake --add-missing"
automake --add-missing
echo "  autoconf"
autoconf

if [ -x config.status -a -z "$*" ]; then
	./config.status --recheck
else
    if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish"
        echo "to pass any to it, please specify them on the $0  command line."
        echo "If you do not wish to run ./configure, press  Ctrl-C now."
        trap 'echo "configure aborted" ; exit 0' 1 2 15
        sleep 1
    fi
    echo "  ./configure ${@}"
    ./configure "$@";
fi
