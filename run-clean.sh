#!/bin/sh
make clean
make distclean
rm -rvf aclocal.m4 autom4te.cache compile config.h config.h.in config.log config.status configure depcomp install-sh Makefile Makefile.in missing stamp-h1 config.guess config.sub
rm src/Makefile.in

