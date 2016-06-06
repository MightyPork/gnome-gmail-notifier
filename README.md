# Gnome Gmail Notifier

This is an unofficial fork to make it work on Arch and to add some features I've been missing.

Feel free to use it.

**Upstream:** (archived) Google Code project at https://code.google.com/archive/p/gnome-gmail-notifier/

## What's added

- Left-click on the icon runs `xdg-open "https://mail.google.com"` to open your inbox.
- Notification has an "Open" button (to do the same)

## PKGBUILD

```bash
pkgname=gnome-gmail-notifier

# Set a version
pkgver=0.10.2
pkgrel=1

pkgdesc="A Gmail Inbox Monitor for the GNOME2 desktop (MATE) - improved version"
arch=('i686' 'x86_64')
license=('GPL2')
depends=('gconf' 'gstreamer0.10' 'gtk2' 'libgnome-keyring' 'libnotify' 'libsoup' 'gnome-keyring')
makedepends=('intltool')

# Point to a source tarball, and update the pkgsum
source=(gnome-gmail-notifier-src.tar.gz)
md5sums=('329331706834c3d99f82f0d3cf686dae')

prepare() {
  cd $srcdir/$pkgname-$pkgver
}

build() {
  cd "$pkgname-$pkgver"

  ./autogen.sh
  ./configure --prefix=/usr
  make -j4
}

package() {
  cd "$pkgname-$pkgver"
  make DESTDIR="$pkgdir/" install
}
```


---

## Original README

The GNOME Gmail Notifier is a project designed to provide a simple, easy-
to-use application that monitors users' gmail inbox(es), and to deliver
the appropriate notifications when new mail exists.

In order to use the GNOME Gmail Notifier, you'll need the following
prerequesite packages, whether compiled from source or not:

  * GLib 2.6.0
  * GTK+ 2.12.0
  * GConf 2.20.0
  * Libxml2 2.6.20
  * Libsoup 2.6.0
  * Libnotify 0.4.2
  * GStreamer 0.10.0
  * Gnome Keyring 0.4.2

When compiling from subversion, please use './autogen.sh --prefix=/usr',
'make', and 'sudo make install' to install from source. Users installing
from source tarballs may replace the 'autogen.sh' with 'configure' in the
above commands to perform the same tasks.

This project codebase has sustained three successive rewrites, and each
has introduced numerous, massive improvements into the project. The
current codebase is written using GLib and GObject, and utilizes the
GLib multithreading capability to monitor multiple user accounts. Recent
improvements have yielded support for proxy-based connections and faster,
more reliable mail checking routines, as well as a more sturdy graphical
user interface. Finally, the last rewrite nailed down the complete
internal functionality of the notifier in the most logical way possible,
ensuring that stability and ease of modification are the top priority.
In short, the project is quite mature.

Please email me with questions at <geekysuavo@gmail.com>, or post issues
at <http://code.google.com/p/gnome-gmail-notifier/issues/list>. Anyone
wanting to contribute ideas, code, or translations may email me directly.
The latest subversion copy is (currently) located at google code, and
the latest tarballs are hosted at <http://notifier.geekysuavo.org/>.

thanks for reading. enjoy the notifier.
  ~ brad.
