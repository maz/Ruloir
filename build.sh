#!/bin/sh
mkdir -p bin

if [ -z $CC ]; then
	if type clang >> /dev/null 2> /dev/null; then
		CC="/usr/bin/env clang"
	else
		CC="/usr/bin/env gcc"
	fi
fi

CFLAGS="$CFLAGS -std=c99"

if [ -f /usr/local/include/mongo.h ]; then
	CFLAGS="$CFLAGS -I/usr/local/include -DMONGO_CLIENT_INSTALLED"
	LDFLAGS="$LDFLAGS -L/usr/local/include -lbson -lmongoc"
elif  [ -f /usr/include/mongo.h ]; then
	CFLAGS="$CFLAGS -I/usr/include -DMONGO_CLIENT_INSTALLED"
	LDFLAGS="$LDFLAGS -L/usr/include -lbson -lmongoc"
fi

CFLAGS="$CFLAGS -I/usr/local/include -DREENTRANT"
LDFLAGS="$LDFLAGS -L/usr/local/lib -ljemalloc -pthread -ldl"

$CC -g -o bin/ruloir-server $CFLAGS $LDFLAGS src/ruloir-server/*.c &&	\
$CC -g -o bin/default-app.so -Isrc $CFLAGS $LDFLAGS -fPIC -shared src/default-app/default-app.c && \
$CC -g -o bin/default-app2.so -Isrc $CFLAGS $LDFLAGS -fPIC -shared src/default-app/default-app2.c && \
true

exit $?