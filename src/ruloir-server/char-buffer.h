#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H

#include "prefix.h"

#define CHAR_BUFFER_SIZE	(256)

typedef struct CharBuffer{
	int idx;
	int len;
	char buffer[CHAR_BUFFER_SIZE];
} CharBuffer;

#define CHAR_BUFFER_INITIALIZER		{0}

char CharBufferRead(int fd,CharBuffer *buf);

#endif