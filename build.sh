#!/bin/sh
if [ -z $CC ]; then
	if which clang >> /dev/null; then
		CC="/usr/bin/env clang"
	else
		CC="/usr/bin/env gcc"
	fi
fi

CFLAGS="-I/usr/local/include -DREENTRANT"
LDFLAGS="-L/usr/local/lib -ljemalloc -pthread"

$CC -g -o ruloir-server $CFLAGS $LDFLAGS *.c