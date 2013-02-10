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

$CC -g -o bin/ruloir-server $CFLAGS $LDFLAGS src/ruloir-server/*.c &&	\
$CC -g -o bin/default-app.so -Isrc $CFLAGS $LDFLAGS -fPIC -shared src/default-app/default-app.c && \
$CC -g -o bin/default-app2.so -Isrc $CFLAGS $LDFLAGS -fPIC -shared src/default-app/default-app2.c && \
true

exit $?