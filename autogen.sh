#!/bin/sh

aclocal
automake -ac
autoconf
echo "Ready to go (run configure)"
