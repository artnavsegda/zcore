#!/bin/sh
./run-clean.sh
autoreconf -i
./configure
make

