AC_INIT(Makefile.am)
AM_INIT_AUTOMAKE(eye-on-manga, 0.1)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_INSTALL
AC_HEADER_STDC

# Option to enable debugging
AC_ARG_ENABLE(debug,
                [AC_HELP_STRING([ --enable-debug], [Debugging (default=no)])],
                [with_debug=yes], [with_debug=no])

if test "x$with_debug" == "xyes" ; then
   CFLAGS="$CFLAGS -ggdb -O0 -DDEBUG -Wall "
else
   CFLAGS="$CFLAGS -O2 -Wall "
fi

# Hildon library dependencies
PKG_CHECK_MODULES(HILDON, hildon-1)
AC_SUBST(HILDON_LIBS)
AC_SUBST(HILDON_CFLAGS)

# To make the application visible in maemo Task Navigator it needs a Desktop
# file for the application.
desktopentrydir=`$PKG_CONFIG osso-af-settings --variable=desktopentrydir`

# Application pixmaps install directory
pixmapdir=`$PKG_CONFIG osso-af-settings --variable=hildonpixmapdir`

# Application icon install directories
icon_26x26dir=$datadir/icons/hicolor/26x26/hildon
icon_34x34dir=$datadir/icons/hicolor/34x34/hildon
icon_40x40dir=$datadir/icons/hicolor/40x40/hildon
icon_50x50dir=$datadir/icons/hicolor/50x50/hildon
icon_scalabledir=$datadir/icons/hicolor/scalable/hildon

# Define as variables in Makefile
AC_SUBST(desktopentrydir)
AC_SUBST(pixmapdir)
AC_SUBST(icon_26x26dir)
AC_SUBST(icon_34x34dir)
AC_SUBST(icon_40x40dir)
AC_SUBST(icon_50x50dir)
AC_SUBST(icon_scalabledir)

AC_DEFINE_UNQUOTED([PIXMAPDIR], "${pixmapdir}", [Runtime pixmap files path])
AC_OUTPUT(
        Makefile \
        src\Makefile \
        data\Makefile
)
