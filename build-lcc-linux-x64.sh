#!/bin/bash

export BUILDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/bld/x86/linux"
mkdir -p $BUILDDIR
ln -Tsfr ./include/x86/linux $BUILDDIR/include
cp -Tr /usr/lib/gcc/*/* $BUILDDIR/gcc
if [ ! -e $BUILDDIR/gcc/cpp ]; then
	sudo ln -sf `which cpp` $BUILDDIR/gcc/cpp
fi

make clean
make HOSTFILE=etc/linux.c CFLAGS='-DLCCDIR=\"$BUILDDIR\"' lcc
make all
make TARGET=x86/linux test

echo "If you want to run tests then run 'export BUILDDIR=$BUILDDIR' first"