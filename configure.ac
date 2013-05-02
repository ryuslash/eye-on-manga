## Eye on Manga - Manga collection software for Maemo 5
# Copyright (C) 2010-2013  Tom Willemse

# Eye on Manga is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Eye on Manga is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with Eye on Manga.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT(Makefile.am)
AM_INIT_AUTOMAKE(eye-on-manga, 0.1.0)

AC_PROG_CC
AC_PROG_INSTALL

# Check for necessary packages
PKG_CHECK_MODULES(EOM, hildon-1 sqlite3)
# Add packages to flags and libs
CFLAGS="-Wall -Wextra $EOM_CFLAGS $CFLAGS"
LIBS="$EOM_LIBS $LIBS"

desktopentrydir=`$PKG_CONFIG osso-af-settings --variable=desktopentrydir`

# Application icon install directories
icon_26x26dir=$datadir/icons/hicolor/26x26/hildon
icon_40x40dir=$datadir/icons/hicolor/40x40/hildon
icon_64x64dir=$datadir/icons/hicolor/64x64/hildon
icon_scalabledir=$datadir/icons/hicolor/scalable/hildon

AC_SUBST(desktopentrydir)
AC_SUBST(icon_26x26dir)
AC_SUBST(icon_40x40dir)
AC_SUBST(icon_64x64dir)
AC_SUBST(icon_scalabledir)

# Generate the Makefile from Makefile.in
AC_OUTPUT( \
           Makefile \
           src/Makefile \
           data/Makefile
)
