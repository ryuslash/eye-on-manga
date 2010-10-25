#!/bin/sh

if test -f Makefile; then
    make distclean
fi

rm -f *.tar.* *.tgz
rm -Rf autom4te.cache
rm -f src/Makefile.in data/Makefile.in Makefile.in aclocal.m4 configure depcomp install-sh missing build-stamp configure-stamp
