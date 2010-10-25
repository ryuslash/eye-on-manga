AC_INIT(Makefile.am)
AM_INIT_AUTOMAKE(eye-on-manga, 0.0.1)

AC_PROG_CC
AC_PROG_INSTALL

# Check for necessary packages
PKG_CHECK_MODULES(EOM, hildon-1 sqlite3)
# Add packages to flags and libs
CFLAGS="$EOM_CFLAGS $CFLAGS"
LIBS="$EOM_LIBS $LIBS"
# Generate the Makefile from Makefile.in
AC_OUTPUT( \
           Makefile \
           src/Makefile
)
