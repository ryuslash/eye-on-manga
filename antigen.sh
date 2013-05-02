#!/bin/sh
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


if test -f Makefile; then
    make distclean
fi

rm -f *.tar.* *.tgz
rm -Rf autom4te.cache
rm -f src/Makefile.in data/Makefile.in Makefile.in aclocal.m4 configure depcomp install-sh missing build-stamp configure-stamp
