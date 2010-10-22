#!/bin/sh

set -x
aclocal-1.7
autoconf --force
autoheader --force
automake-1.7 --add-missing --copy --force-missing --foreign
